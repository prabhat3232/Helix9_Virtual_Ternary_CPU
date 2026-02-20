#include "helix_runtime.h"
#include "../cpu.h"
#include "../memory.h"
#include <iostream>

namespace Helix {

std::vector<TernaryWord> HelixRuntime::Execute(const TNNModel& model, const std::vector<TernaryWord>& input) {
    // 1. Setup Virtual CPU environment for Accelerator
    TernaryMemory mem;
    Cpu cpu(mem);

    // 2. Memory Layout Map
    // We place inputs at 0x1000, outputs at 0x2000, weights at 0x3000
    int64_t input_addr = 0x1000;
    int64_t output_addr = 0x2000;
    int64_t weight_addr = 0x3000;
    
    std::vector<TernaryWord> current_input = input;

    // Process layer by layer
    for (size_t l_idx = 0; l_idx < model.layers.size(); ++l_idx) {
        const auto& layer = model.layers[l_idx];
        
        // Write Input to Memory
        for (size_t i = 0; i < current_input.size(); ++i) {
             mem.Write(TernaryWord::FromInt64(input_addr + i), current_input[i]);
        }

        if (layer.type == "Dense") {
             // 1. Write Weights to Memory
             for (size_t i = 0; i < layer.weights.size(); ++i) {
                 mem.Write(TernaryWord::FromInt64(weight_addr + i), layer.weights[i]);
             }

             // 2. Configure Vector Unit
             // In a real scenario, we'd feed assembly instructions to the CPU.
             // Here, we simulate the `VMMUL` instruction execution wrapper
             // The vector_length must match the layer size.
             cpu.vector_length = layer.input_size;
             
             // Base Rs1 (Input Vector)
             cpu.regs[1] = TernaryWord::FromInt64(input_addr);
             // Base Rs2 (Matrix Base)
             cpu.regs[2] = TernaryWord::FromInt64(weight_addr);
             
             // Execute Sequence:
             // VLDR V0, R1
             cpu.regs[10] = TernaryWord::FromInt64(0x21404000); // Hack: Opcode 32, Mode 0, Rd 0, Rs1 1
             // VMMUL V1, V0, R2
             cpu.regs[11] = TernaryWord::FromInt64(0x24C10800); // Hack: Opcode 36, Mode 0, Rd 1, Rs1 0, Rs2 2
             
             // Instead of assembling, we just call the helper logic directly
             // Or write a small inline assembly to memory and run.
             // Let's use direct C++ simulation for the runtime orchestrator:
             
             std::vector<TernaryWord> v_out(layer.output_size);
             for(int row = 0; row < layer.output_size; ++row) {
                  int64_t sum = 0;
                  int64_t row_base = weight_addr + (row * layer.input_size);
                  
                  for(int col = 0; col < layer.input_size; ++col) {
                       int64_t in_val = current_input[col].ToInt64();
                       int64_t w_val = mem.Read(TernaryWord::FromInt64(row_base + col)).ToInt64();
                       sum += in_val * w_val;
                  }
                  
                  // Apply Bias
                  if (!layer.biases.empty() && row < layer.biases.size()) {
                       sum += layer.biases[row].ToInt64();
                  }
                  
                  v_out[row] = TernaryWord::FromInt64(sum);
             }
             
             current_input = v_out; // Pass to next layer
             
        } else if (layer.type == "Activation_Sign") {
             // VSIGN Vd, Vs
             std::vector<TernaryWord> v_out(layer.output_size);
             for(int i = 0; i < current_input.size(); ++i) {
                  int64_t val = current_input[i].ToInt64();
                  int64_t sign = (val > 0) ? 1 : ((val < 0) ? -1 : 0);
                  v_out[i] = TernaryWord::FromInt64(sign);
             }
             current_input = v_out;
        } else if (layer.type == "Activation_Clip") {
             // VCLIP
             std::vector<TernaryWord> v_out(layer.output_size);
             int64_t limit = 1; // Default
             for(int i = 0; i < current_input.size(); ++i) {
                  int64_t val = current_input[i].ToInt64();
                  if (val > limit) val = limit;
                  if (val < -limit) val = -limit;
                  v_out[i] = TernaryWord::FromInt64(val);
             }
             current_input = v_out;
        }
    }

    return current_input; // Final output
}

} // namespace Helix
