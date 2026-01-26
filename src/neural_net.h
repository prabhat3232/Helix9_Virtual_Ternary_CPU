#include "sparse_math.h"
#include "vector_alu.h"
#include <vector>
#include <random>
#include <fstream>
#include <iostream>

class TernaryLayer {
public:
    int input_size;
    int output_size;
    SparseMatrix weights;
    std::vector<TernaryFloat> biases;
    
    // Optimizations: Parallel Vectors (SoA)
    // These match the topology of 'weights' exactly.
    // weight_grads[i][k] corresponds to weights.row_vectors[i].indices[k]
    std::vector<std::vector<double>> weight_grads; 
    std::vector<double> bias_grads;

    TernaryLayer(int in, int out, double sparsity) : input_size(in), output_size(out), weights(out, in) {
        biases.resize(out, TernaryFloat(0,0));
        bias_grads.resize(out, 0.0);
        
        // ---------- TOPOLOGY-AWARE SPARSITY INITIALIZATION ----------
        // Phase 1: Guarantee at least 1 connection per INPUT neuron
        // This prevents "Sparse Starvation" where an input has no outgoing connections
        for(int j=0; j<in; ++j) {
            int random_output = rand() % out;
            double r = ((rand() % 200) - 100) / 100.0;
            weights.row_vectors[random_output].Set(j, TernaryFloat::FromDouble(r));
        }
        
        // Phase 2: Fill remaining connections randomly up to target sparsity
        int target_connections = (int)(sparsity * in * out);
        int current_connections = in; // Already have 'in' guaranteed connections
        
        while (current_connections < target_connections) {
            int i = rand() % out;  // Random output neuron
            int j = rand() % in;   // Random input neuron
            
            // Check if connection already exists
            bool exists = false;
            for(size_t k=0; k<weights.row_vectors[i].indices.size(); ++k) {
                if (weights.row_vectors[i].indices[k] == j) {
                    exists = true;
                    break;
                }
            }
            
            if (!exists) {
                double r = ((rand() % 200) - 100) / 100.0;
                weights.row_vectors[i].Set(j, TernaryFloat::FromDouble(r));
                current_connections++;
            }
        }
        
        // Init Biases
        for(int i=0; i<out; ++i) {
            biases[i] = TernaryFloat::FromDouble(0.5);
        }

        // Initialize Gradients based on the created sparsity topology
        weight_grads.resize(out);
        for(int i=0; i<out; ++i) {
            weight_grads[i].resize(weights.row_vectors[i].indices.size(), 0.0);
        }
    }

    std::vector<TernaryFloat> Forward(const std::vector<TernaryFloat>& inputs) {
        std::vector<TernaryFloat> outputs;
        for(int i=0; i<output_size; ++i) {
            TernaryFloat dot = weights.DotRow(i, inputs);
            outputs.push_back(dot.Add(biases[i]).ReLU());
        }
        return outputs;
    }
};

class TSAI_Model {
public:
    std::vector<TernaryLayer> layers;
    double learning_rate = 0.1;

    // Optimizer State (Momentum)
    // Matches topology: weight_velocities[layer][out][k]
    std::vector<std::vector<std::vector<double>>> weight_velocities; 
    std::vector<std::vector<double>> bias_velocities; // [Layer][Out]
    double momentum = 0.9;

    void AddLayer(int in, int out, double sparsity = 1.0) {
        layers.push_back(TernaryLayer(in, out, sparsity));
        
        // Init Velocities to match topology
        auto& last_layer = layers.back();
        std::vector<std::vector<double>> layer_vels;
        layer_vels.resize(out);
        
        for(int i=0; i<out; ++i) {
            layer_vels[i].resize(last_layer.weights.row_vectors[i].indices.size(), 0.0);
        }
        weight_velocities.push_back(layer_vels);
        
        bias_velocities.push_back(std::vector<double>(out, 0.0));
    }

    std::vector<TernaryFloat> Forward(const std::vector<TernaryFloat>& input) {
        std::vector<TernaryFloat> curr = input;
        for(auto& layer : layers) {
            curr = layer.Forward(curr);
        }
        return curr;
    }
    
    void Train(const std::vector<TernaryFloat>& input, const std::vector<TernaryFloat>& target) {
        // 1. Forward Pass
        std::vector<std::vector<TernaryFloat>> activations;
        activations.push_back(input);
        
        std::vector<TernaryFloat> curr = input;
        std::vector<std::vector<TernaryFloat>> pre_activations; 

        for(auto& layer : layers) {
            std::vector<TernaryFloat> z_layer;
            std::vector<TernaryFloat> a_layer;
            for(int i=0; i<layer.output_size; ++i) {
                TernaryFloat dot = layer.weights.DotRow(i, curr);
                TernaryFloat z = dot.Add(layer.biases[i]);
                z_layer.push_back(z);
                a_layer.push_back(z.ReLU());
            }
            pre_activations.push_back(z_layer);
            activations.push_back(a_layer);
            curr = a_layer;
        }

        // 2. Backward Pass
        std::vector<double> next_layer_gammas; 
        
        for (int l = layers.size() - 1; l >= 0; --l) {
            TernaryLayer& layer = layers[l];
            const auto& output = activations[l+1]; 
            const auto& input_to_layer = activations[l]; 
            const auto& z_vals = pre_activations[l];
            
            std::vector<double> current_gammas;
            
            for(int i=0; i<layer.output_size; ++i) {
                double z = z_vals[i].ToDouble();
                double relu_prime = (z > 0) ? 1.0 : 0.01; 
                
                double error_term = 0.0;
                
                if (l == layers.size() - 1) {
                    double y = output[i].ToDouble();
                    double t = target[i].ToDouble();
                    error_term = (y - t);
                } else {
                    TernaryLayer& next_layer = layers[l+1];
                    // Sparse optimization: only iterate active connections of the NEXT layer
                    // to find contributions to CURRENT neuron i
                    // Sum(Gamma_next_k * W_next_ki)
                    
                    // Note: This is efficient for Row-wise, but finding W_ki (all K connected to I) 
                    // is actually difficult in CSR without a transpose or full scan.
                    // For PoC/Learning, we scan next layer's rows.
                    // (Optimization: Maintain CSC or adjacency list for backprop, but costly for today).
                    
                    for(int k=0; k<next_layer.output_size; ++k) {
                        // Does next_layer neuron 'k' connect to this neuron 'i'?
                        // Search its indices.
                        const auto& vec = next_layer.weights.row_vectors[k];
                        for(size_t w_idx=0; w_idx < vec.indices.size(); ++w_idx) {
                             if (vec.indices[w_idx] == i) {
                                 double w_ki = vec.values[w_idx].ToDouble();
                                 error_term += next_layer_gammas[k] * w_ki;
                                 break; // Found the connection
                             }
                        }
                    }
                }
                
                double gamma = error_term * relu_prime;
                current_gammas.push_back(gamma);
                
                layer.bias_grads[i] += gamma;
                
                // Weight Grads = Gamma * PreviousActivation
                // Iterate ONLY existing connections (Structure is fixed during training step)
                const auto& vec = layer.weights.row_vectors[i];
                for(size_t k=0; k<vec.indices.size(); ++k) {
                    int col = vec.indices[k]; // Index of input neuron
                    double a_prev = input_to_layer[col].ToDouble();
                    
                    // We can also skip if a_prev is 0, but that's data sparsity, not weight sparsity.
                    if (a_prev != 0) {
                        layer.weight_grads[i][k] += gamma * a_prev;
                    }
                }
            }
            next_layer_gammas = current_gammas;
        }
    }
    
    void ApplyUpdates() {
        for(size_t l=0; l<layers.size(); ++l) {
            auto& layer = layers[l];
            for(int i=0; i<layer.output_size; ++i) {
                // --- Update Bias ---
                double b_grad = layer.bias_grads[i];
                double b_vel = momentum * bias_velocities[l][i] - learning_rate * b_grad;
                bias_velocities[l][i] = b_vel;
                
                double b_curr = layer.biases[i].ToDouble();
                layer.biases[i] = TernaryFloat::FromDouble(b_curr + b_vel);
                layer.bias_grads[i] = 0; 
                
                // --- Update Weights (Sparse Parallel Vectors) ---
                auto& vec = layer.weights.row_vectors[i];
                
                // Iterate ALL connections 'k'. 
                // CRITICAL: Even if grad is 0, we must decay velocity.
                for(size_t k=0; k < vec.indices.size(); ++k) {
                    double w_grad = layer.weight_grads[i][k];
                    
                    // Clip Grad
                    if (w_grad > 5.0) w_grad = 5.0;
                    if (w_grad < -5.0) w_grad = -5.0;

                    // Momentum Update
                    double& w_vel = weight_velocities[l][i][k];
                    w_vel = momentum * w_vel - learning_rate * w_grad;
                    
                    // Clip Vel
                    if (w_vel > 1.0) w_vel = 1.0;
                    if (w_vel < -1.0) w_vel = -1.0;
                    
                    // Apply to Weight
                    TernaryFloat& w_val_t = vec.values[k];
                    double w_curr = w_val_t.ToDouble();
                    double w_new = w_curr + w_vel;
                    
                    if (w_new > 5.0) w_new = 5.0;
                    if (w_new < -5.0) w_new = -5.0;
                    
                    // Note: In fixed topology, we do not prune here. 
                    // We just update the value. Real pruning requires structure change.
                    // For now, allow it to cross 0.
                    w_val_t = TernaryFloat::FromDouble(w_new);
                    
                    // Reset Grad
                    layer.weight_grads[i][k] = 0; 
                }
            }
        }
    }

    void Save(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) return;
        
        file << layers.size() << "\n";
        for(auto& layer : layers) {
            file << layer.input_size << " " << layer.output_size << "\n";
            for(const auto& b : layer.biases) {
                file << b.mantissa << " " << b.exponent << " ";
            }
            file << "\n";
            for(int i=0; i<layer.output_size; ++i) {
                const auto& vec = layer.weights.row_vectors[i];
                file << vec.indices.size() << " ";
                for(size_t k=0; k<vec.indices.size(); ++k) {
                    file << vec.indices[k] << " " << vec.values[k].mantissa << " " << vec.values[k].exponent << " ";
                }
                file << "\n";
            }
        }
        file.close();
    }

    void Load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return;
        
        int layer_count;
        file >> layer_count;
        layers.clear();
        weight_velocities.clear();
        bias_velocities.clear();
        
        for(int l=0; l<layer_count; ++l) {
            int in, out;
            file >> in >> out;
            AddLayer(in, out); // This inits zero velocities and random weights
            
            // Overwrite with loaded data
            for(int i=0; i<out; ++i) {
                int64_t m, e;
                file >> m >> e;
                layers.back().biases[i] = TernaryFloat(m, e);
            }
            
            for(int i=0; i<out; ++i) {
                int count;
                file >> count;
                 auto& vec = layers.back().weights.row_vectors[i];
                 vec.indices.clear();
                 vec.values.clear();
                 
                 // We also need to resize our gradients/velocities because we just blew away the structure
                 // But wait, AddLayer created the structure. We are loading potentially DIFFERENT structure.
                 // So we must rebuild the aux vectors.
                 
                for(int k=0; k<count; ++k) {
                    int col;
                    int64_t m, e;
                    file >> col >> m >> e;
                    vec.indices.push_back(col);
                    vec.values.push_back(TernaryFloat(m, e));
                }
                
                // Re-init aux vectors for this neuron to match loaded topology
                layers.back().weight_grads[i].assign(count, 0.0);
                weight_velocities.back()[i].assign(count, 0.0);
            }
        }
        file.close();
    }
};
