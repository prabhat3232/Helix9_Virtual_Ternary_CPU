#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include "../trit_word.h"

namespace Helix {

// Represents a layer in the Ternary Neural Network
struct TNNLayer {
    std::string type; // "Dense", "Conv2D", "Activation_Sign", etc.
    int input_size;
    int output_size;
    std::vector<TernaryWord> weights; // Packed ternary weights
    std::vector<TernaryWord> biases;  // Biases (optional)
};

// Represents the full Ternary Neural Network Model
struct TNNModel {
    int version;
    std::string name;
    std::vector<TNNLayer> layers;
};

class HelixModelLoader {
public:
    // Load a .htnn (Helix Ternary Neural Network) model file
    static TNNModel LoadModel(const std::string& filepath);
    
    // Save a model to a .htnn file
    static bool SaveModel(const TNNModel& model, const std::string& filepath);
};

class HelixRuntime {
public:
    // Executes a loaded TNN model on an input vector using the CPU Vector Unit
    // Returns the output vector.
    static std::vector<TernaryWord> Execute(const TNNModel& model, const std::vector<TernaryWord>& input);
};

} // namespace Helix
