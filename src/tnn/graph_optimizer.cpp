#include "graph_optimizer.h"
#include "../cpu.h"
#include "../isa.h"
#include <iostream>

namespace Helix {

// Helper to encode instruction for Codegen pass
static TernaryWord CodegenEncode(int opcode, int mode, int rd, int rs1, int rs2_imm) {
    TernaryWord word;
    word.SetSlice(21, 6, opcode);
    word.SetSlice(18, 3, mode);
    word.SetSlice(14, 4, rd);
    word.SetSlice(10, 4, rs1);
    word.SetSlice(0, 10, rs2_imm);
    return word;
}

// Pass 1: Semantic -> IR
std::vector<GraphNode> GraphCompiler::BuildIR(const TNNModel& model) {
    std::vector<GraphNode> ir;
    
    // Abstractly, we know weights must be loaded, then computations happen.
    // In our model, each dense layer has weights.
    
    for (size_t l_idx = 0; l_idx < model.layers.size(); ++l_idx) {
        const auto& layer = model.layers[l_idx];
        
        if (layer.type == "Dense") {
            // Node 1: Load Weights to Memory is implicit in the memory planner,
            // but representing the operation explicitly helps Codegen if needed.
            // For now, let's represent the actual computations.
            GraphNode op;
            op.type = IROpType::VMMUL;
            op.dim_input = layer.input_size;
            op.dim_output = layer.output_size;
            op.debug_name = "Dense_" + std::to_string(l_idx);
            ir.push_back(op);
        } else if (layer.type == "Activation_Sign") {
            GraphNode op;
            op.type = IROpType::VSIGN;
            op.dim_input = layer.input_size;
            op.dim_output = layer.output_size;
            op.debug_name = "Sign_" + std::to_string(l_idx);
            ir.push_back(op);
        } else if (layer.type == "Activation_Clip") {
            GraphNode op;
            op.type = IROpType::VCLIP;
            op.dim_input = layer.input_size;
            op.dim_output = layer.output_size;
            op.imm_val = 1; // Default
            op.debug_name = "Clip_" + std::to_string(l_idx);
            ir.push_back(op);
        }
    }
    
    // Link prev/next pointers for Fusion Pass
    for (size_t i = 0; i < ir.size(); ++i) {
        ir[i].prev_node = (i > 0) ? &ir[i-1] : nullptr;
        ir[i].next_node = (i < ir.size() - 1) ? &ir[i+1] : nullptr;
    }
    
    return ir;
}

// Pass 2: Fusion Analysis
bool GraphCompiler::CanFuse(const GraphNode& a, const GraphNode& b) {
    if (a.type == IROpType::VMMUL && b.type == IROpType::VSIGN) {
        // Shapes must match
        if (a.dim_output != b.dim_input) return false;
        
        // No intermediate use (Guaranteed since DAG is currently a straight line list)
        // In a true graph, check out-degree of Node A.
        return true; 
    }
    return false;
}

std::vector<GraphNode> GraphCompiler::OptimizeIR(const std::vector<GraphNode>& ir) {
    std::vector<GraphNode> opt;
    
    for (size_t i = 0; i < ir.size(); ++i) {
        if (i < ir.size() - 1 && CanFuse(ir[i], ir[i+1])) {
            // Fuse them
            GraphNode fused;
            fused.type = IROpType::VMMSGN;
            fused.dim_input = ir[i].dim_input;
            fused.dim_output = ir[i].dim_output;
            fused.debug_name = ir[i].debug_name + "_Fused_" + ir[i+1].debug_name;
            opt.push_back(fused);
            i++; // Skip the next node because it has been absorbed
        } else {
            opt.push_back(ir[i]);
        }
    }
    
    // Relink
    for (size_t i = 0; i < opt.size(); ++i) {
        opt[i].prev_node = (i > 0) ? &opt[i-1] : nullptr;
        opt[i].next_node = (i < opt.size() - 1) ? &opt[i+1] : nullptr;
    }
    
    return opt;
}

// Pass 3: Memory Planning Constraint Solver (Linear Strategy)
void GraphCompiler::PlanMemory(std::vector<GraphNode>& ir, TernaryMemory& mem, const TNNModel& model, int64_t input_base_addr, int64_t tensor_base_addr) {
    int64_t current_heap = tensor_base_addr;
    int64_t current_data = input_base_addr;
    
    // 1. Plan Weights (They go directly to heap)
    int layer_model_idx = 0;
    for (auto& node : ir) {
        if (node.type == IROpType::VMMUL || node.type == IROpType::VMMSGN) {
            // Find corresponding model layer to get actual weights
            // (Assuming 1-to-1 ordered mapping for Dense layers)
            while(layer_model_idx < model.layers.size() && model.layers[layer_model_idx].type != "Dense") {
                layer_model_idx++;
            }
            if (layer_model_idx < model.layers.size()) {
                node.weight_addr = current_heap;
                const auto& w = model.layers[layer_model_idx].weights;
                
                // Write weights to planned memory
                for(size_t i=0; i<w.size(); ++i) {
                    mem.Write(TernaryWord::FromInt64(current_heap + i), w[i]);
                }
                
                // Advance Heap (Align to next page optionally? No, let's keep it dense for now)
                current_heap += w.size();
                layer_model_idx++;
            }
        }
    }
    
    // 2. Plan Activations (Linear Ping-Pong or Stack strategy)
    for(size_t i=0; i < ir.size(); ++i) {
        ir[i].input_addr = current_data;
        
        // Allocate buffer for output
        int64_t next_data = current_heap; 
        current_heap += ir[i].dim_output;
        
        ir[i].output_addr = next_data;
        
        // The output of this node is the input of the next node
        current_data = next_data;
    }
}

// Pass 4: Codegen
void GraphCompiler::GenerateProgram(const std::vector<GraphNode>& ir, TernaryMemory& mem, int64_t program_base) {
    int64_t pc = program_base;
    
    for (const auto& node : ir) {
        // Input Base is in R1
        mem.Write(TernaryWord::FromInt64(pc++), CodegenEncode((int)Opcode::LDI, 1, 1, 0, node.input_addr));
        
        // Output Base is implicit / we might need it for VSTR
        
        if (node.type == IROpType::VMMUL || node.type == IROpType::VMMSGN) {
            // Weight Base is in R2
            mem.Write(TernaryWord::FromInt64(pc++), CodegenEncode((int)Opcode::LDI, 1, 2, 0, node.weight_addr));
            
            // Set Cpu Vector Length (We must inject a way to set vector length! Currently fixed by cpu.vector_length?)
            // Wait, VLDR uses cpu.vector_length. If dimensions change, we need a new instruction to set vector length.
            // For now, let's assume dim_input/dim_output are uniform or VSTR handles it.
            // Actually, we should probably add a VSETL opcode to set hardware vector length,
            // but for now compiler assumes CPU is configured properly if uniform,
            // or we just inject it somehow.
            
            // 1. VLDR V0, R1 (Load Vector from Input Addr)
            mem.Write(TernaryWord::FromInt64(pc++), CodegenEncode((int)Opcode::VLDR, 0, 0, 0, 1));
            
            // 2. Process
            int op_val = (node.type == IROpType::VMMSGN) ? (int)Opcode::VMMSGN : (int)Opcode::VMMUL;
            mem.Write(TernaryWord::FromInt64(pc++), CodegenEncode(op_val, 0, 1, 0, 2)); // VMMUL V1, V0, R2
            
            // 3. Store Output
            mem.Write(TernaryWord::FromInt64(pc++), CodegenEncode((int)Opcode::LDI, 1, 3, 0, node.output_addr));
            mem.Write(TernaryWord::FromInt64(pc++), CodegenEncode((int)Opcode::VSTR, 0, 1, 0, 3)); // VSTR V1, R3
            
        } else if (node.type == IROpType::VSIGN) {
            mem.Write(TernaryWord::FromInt64(pc++), CodegenEncode((int)Opcode::VLDR, 0, 0, 0, 1));
            mem.Write(TernaryWord::FromInt64(pc++), CodegenEncode((int)Opcode::VSIGN, 0, 1, 0, 0)); // VSIGN V1, V0
            mem.Write(TernaryWord::FromInt64(pc++), CodegenEncode((int)Opcode::LDI, 1, 3, 0, node.output_addr));
            mem.Write(TernaryWord::FromInt64(pc++), CodegenEncode((int)Opcode::VSTR, 0, 1, 0, 3)); 
            
        } else if (node.type == IROpType::VCLIP) {
            mem.Write(TernaryWord::FromInt64(pc++), CodegenEncode((int)Opcode::VLDR, 0, 0, 0, 1));
            mem.Write(TernaryWord::FromInt64(pc++), CodegenEncode((int)Opcode::VCLIP, 1, 1, 0, node.imm_val)); // VCLIP V1, V0, Imm
            mem.Write(TernaryWord::FromInt64(pc++), CodegenEncode((int)Opcode::LDI, 1, 3, 0, node.output_addr));
            mem.Write(TernaryWord::FromInt64(pc++), CodegenEncode((int)Opcode::VSTR, 0, 1, 0, 3)); 
        }
    }
    
    // HLT
    mem.Write(TernaryWord::FromInt64(pc++), CodegenEncode((int)Opcode::HLT, 0, 0, 0, 0));
}

} // namespace Helix
