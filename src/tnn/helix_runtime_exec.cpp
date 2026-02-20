#include "helix_runtime.h"
#include "../cpu.h"
#include "../memory.h"
#include "graph_optimizer.h"
#include <iostream>

namespace Helix {

std::vector<TernaryWord> HelixRuntime::Execute(const TNNModel& model, const std::vector<TernaryWord>& input) {
    // 1. Setup Virtual CPU environment for Accelerator
    TernaryMemory mem;
    Cpu cpu(mem);

    // 2. Compilation Passes
    std::cout << "[Compiler] Pass 1: Building Graph IR..." << std::endl;
    std::vector<GraphNode> ir = GraphCompiler::BuildIR(model);
    
    std::cout << "[Compiler] Pass 2: Optimizing IR (Fusion)..." << std::endl;
    std::vector<GraphNode> optimized_ir = GraphCompiler::OptimizeIR(ir);
    
    std::cout << "[Compiler] Pass 3: Static Memory Planning..." << std::endl;
    int64_t input_addr = 0x1000;
    int64_t tensor_addr = 0x2000;
    int64_t program_addr = 0x4000;
    GraphCompiler::PlanMemory(optimized_ir, mem, model, input_addr, tensor_addr);
    
    std::cout << "[Compiler] Pass 4: Generating Program Opcodes..." << std::endl;
    GraphCompiler::GenerateProgram(optimized_ir, mem, program_addr);
    
    // 3. Execution Phase
    // Write initial input to the planned input address of the first node
    int64_t first_input_addr = optimized_ir.empty() ? input_addr : optimized_ir[0].input_addr;
    for (size_t i = 0; i < input.size(); ++i) {
         mem.Write(TernaryWord::FromInt64(first_input_addr + i), input[i]);
    }
    
    // Set Vector Length for the CPU (Hardware config assumed uniform for now)
    // In a real Mixed-Dimensional model, we need a VSETL opcode injected 
    // inside GenerateProgram. For Phase 11, we set it manually before run.
    if (!model.layers.empty()) {
        cpu.vector_length = model.layers[0].input_size; // Or extract from IR
    }
    
    std::cout << "[Runtime] Executing Compiled Graph on Virtual CPU at 0x4000..." << std::endl;
    cpu.pc = TernaryWord::FromInt64(program_addr);
    cpu.halted = false;
    cpu.trace_enabled = false;
    cpu.metrics.active_cycles = 0;
    cpu.Run(10000); // 10k max cycles safety limit
    
    std::cout << "[Runtime] Execution Complete. Active Cycles: " << cpu.metrics.active_cycles << std::endl;
    
    // 4. Extract Output
    std::vector<TernaryWord> result;
    if (!optimized_ir.empty()) {
        const GraphNode& last_node = optimized_ir.back();
        int out_size = last_node.dim_output;
        int64_t out_addr = last_node.output_addr;
        
        result.resize(out_size);
        for(int i = 0; i < out_size; ++i) {
            result[i] = mem.Read(TernaryWord::FromInt64(out_addr + i));
        }
    }
    
    return result;
}

} // namespace Helix
