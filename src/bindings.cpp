#include "cpu.h"
#include "neural_net.h"
#include <iostream>

// Global State Holders
static TernaryMemory* global_mem = nullptr;
static Cpu* global_cpu = nullptr;
static TSAI_Model* global_ai = nullptr;

extern "C" {
    // --- CPU API ---
    __declspec(dllexport) void Helix_CreateCPU() {
        if (global_cpu) delete global_cpu;
        if (global_mem) delete global_mem;
        
        global_mem = new TernaryMemory();
        global_cpu = new Cpu(*global_mem);
        
        // Init minimum memory
        global_mem->Write(TernaryWord::FromInt64(0), TernaryWord::FromInt64(0)); 
    }

    __declspec(dllexport) void Helix_DestroyCPU() {
        if (global_cpu) {
            delete global_cpu;
            global_cpu = nullptr;
        }
        if (global_mem) {
            delete global_mem;
            global_mem = nullptr;
        }
    }

    __declspec(dllexport) void Helix_CPU_WriteMem(int addr, int val) {
        if (global_mem) {
            global_mem->Write(TernaryWord::FromInt64(addr), TernaryWord::FromInt64(val));
        }
    }

    __declspec(dllexport) int Helix_CPU_ReadMem(int addr) {
        if (global_mem) {
            TernaryWord w = global_mem->Read(TernaryWord::FromInt64(addr));
            return (int)w.ToInt64();
        }
        return 0;
    }

    __declspec(dllexport) void Helix_CPU_Step() {
        if (global_cpu) {
            global_cpu->Step();
        }
    }

    __declspec(dllexport) int Helix_CPU_GetPC() {
        if (global_cpu) return (int)global_cpu->pc.ToInt64();
        return 0;
    }
    
    __declspec(dllexport) int Helix_CPU_GetRegister(int reg_idx) {
        if (global_cpu && reg_idx >= 0 && reg_idx < 16) {
             return (int)global_cpu->regs[reg_idx].ToInt64();
        }
        return 0;
    }

    // --- AI API ---
    __declspec(dllexport) void Helix_CreateAI() {
        if (global_ai) delete global_ai;
        global_ai = new TSAI_Model();
    }
    
    __declspec(dllexport) void Helix_AI_AddLayer(int in, int out, double sparsity) {
        if (global_ai) {
             global_ai->AddLayer(in, out, sparsity);
        }
    }
    
    __declspec(dllexport) void Helix_AI_SetLR(double lr) {
        if (global_ai) global_ai->learning_rate = lr;
    }

    // Pass arrays from Python: double* inputs, int size
    // WARNING: This applies updates immediately after each sample (Batch Size 1)
    // For better training, use Helix_AI_TrainStep + Helix_AI_ApplyUpdates
    __declspec(dllexport) double Helix_AI_Train(double* inputs, int in_size, double* targets, int out_size) {
        if (!global_ai) return 0.0;
        
        std::vector<TernaryFloat> in_vec;
        for(int i=0; i<in_size; ++i) in_vec.push_back(TernaryFloat::FromDouble(inputs[i]));
        
        std::vector<TernaryFloat> tgt_vec;
        for(int i=0; i<out_size; ++i) tgt_vec.push_back(TernaryFloat::FromDouble(targets[i]));
        
        // Train
        global_ai->Train(in_vec, tgt_vec);
        global_ai->ApplyUpdates();
        
        // Return loss of forward pass
        auto out = global_ai->Forward(in_vec);
        double loss = 0;
        for(int i=0; i<out_size; ++i) {
            double diff = out[i].ToDouble() - targets[i];
            loss += diff*diff;
        }
        return loss;
    }
    
    // NEW: Accumulate gradients without applying (for batching)
    __declspec(dllexport) void Helix_AI_TrainStep(double* inputs, int in_size, double* targets, int out_size) {
        if (!global_ai) return;
        
        std::vector<TernaryFloat> in_vec;
        for(int i=0; i<in_size; ++i) in_vec.push_back(TernaryFloat::FromDouble(inputs[i]));
        
        std::vector<TernaryFloat> tgt_vec;
        for(int i=0; i<out_size; ++i) tgt_vec.push_back(TernaryFloat::FromDouble(targets[i]));
        
        global_ai->Train(in_vec, tgt_vec);
    }
    
    // NEW: Apply accumulated gradients (call after batch of TrainStep calls)
    __declspec(dllexport) void Helix_AI_ApplyUpdates() {
        if (global_ai) global_ai->ApplyUpdates();
    }
    
    __declspec(dllexport) void Helix_AI_Predict(double* inputs, int in_size, double* outputs, int out_size) {
        if (!global_ai) return;
        
        std::vector<TernaryFloat> in_vec;
        for(int i=0; i<in_size; ++i) in_vec.push_back(TernaryFloat::FromDouble(inputs[i]));
        
        auto out = global_ai->Forward(in_vec);
        
        for(int i=0; i<out_size; ++i) {
            if (i < out.size()) outputs[i] = out[i].ToDouble();
        }
    }
    
    // PHASE 3: High-performance training - entire epoch loop in C++
    // all_inputs: flattened array [num_samples * in_size]
    // all_targets: flattened array [num_samples * out_size]
    __declspec(dllexport) double Helix_AI_Fit(
        double* all_inputs,
        double* all_targets,
        int num_samples,
        int in_size,
        int out_size,
        int epochs,
        int log_interval  // Print loss every N epochs (0 = no logging)
    ) {
        if (!global_ai) return -1.0;
        
        // Pre-convert all data to TernaryFloat (once, not per epoch!)
        std::vector<std::vector<TernaryFloat>> inputs(num_samples);
        std::vector<std::vector<TernaryFloat>> targets(num_samples);
        
        for(int s = 0; s < num_samples; ++s) {
            for(int i = 0; i < in_size; ++i) {
                inputs[s].push_back(TernaryFloat::FromDouble(all_inputs[s * in_size + i]));
            }
            for(int i = 0; i < out_size; ++i) {
                targets[s].push_back(TernaryFloat::FromDouble(all_targets[s * out_size + i]));
            }
        }
        
        // Training loop
        double final_loss = 0;
        for(int epoch = 0; epoch < epochs; ++epoch) {
            // Accumulate gradients over all samples
            for(int s = 0; s < num_samples; ++s) {
                global_ai->Train(inputs[s], targets[s]);
            }
            global_ai->ApplyUpdates();
            
            // Calculate loss for logging
            if (log_interval > 0 && epoch % log_interval == 0) {
                double total_loss = 0;
                for(int s = 0; s < num_samples; ++s) {
                    auto out = global_ai->Forward(inputs[s]);
                    for(int i = 0; i < out_size; ++i) {
                        double diff = out[i].ToDouble() - targets[s][i].ToDouble();
                        total_loss += diff * diff;
                    }
                }
                std::cout << "  Epoch " << epoch << ": Loss = " << total_loss << std::endl;
            }
        }
        
        // Final loss calculation
        for(int s = 0; s < num_samples; ++s) {
            auto out = global_ai->Forward(inputs[s]);
            for(int i = 0; i < out_size; ++i) {
                double diff = out[i].ToDouble() - targets[s][i].ToDouble();
                final_loss += diff * diff;
            }
        }
        
        return final_loss;
    }
}
