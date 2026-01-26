#include "neural_net.h"
#include <iostream>
#include <iomanip>
#include <vector>

int main() {
    std::cout << "--- Helix-9 AI Inference Engine ---" << std::endl;
    
    // 1. Initialize empty model
    TSAI_Model model;
    
    // 2. Load trained weights
    std::string model_file = "xor_brain.ternary";
    std::cout << "Loading model from: " << model_file << "..." << std::endl;
    model.Load(model_file);
    
    if (model.layers.empty()) {
        std::cerr << "[Error] Failed to load model or model is empty." << std::endl;
        return 1;
    }
    
    std::cout << "Model Loaded Successfully!" << std::endl;
    std::cout << "Architecture: " << model.layers.size() << " Layers." << std::endl;
    
    // 3. Define Test Data (XOR)
    std::vector<std::pair<std::vector<double>, std::vector<double>>> tests = {
        {{0,0}, {0}},
        {{0,1}, {1}},
        {{1,0}, {1}},
        {{1,1}, {0}}
    };
    
    // 4. Run Inference
    std::cout << "\n--- Running Inference ---" << std::endl;
    int correct = 0;
    
    for(auto& t : tests) {
        std::vector<TernaryFloat> in;
        for(double d : t.first) in.push_back(TernaryFloat::FromDouble(d));
        
        auto out = model.Forward(in);
        double y = out[0].ToDouble();
        
        // Simple threshold for classification
        int predicted = (y > 0.5) ? 1 : 0;
        int expected = (int)t.second[0];
        
        std::cout << "Input: [" << t.first[0] << ", " << t.first[1] << "] "
                  << "-> Raw: " << std::fixed << std::setprecision(4) << y
                  << " -> Pred: " << predicted
                  << " (Exp: " << expected << ") ";
                  
        if (predicted == expected) {
            std::cout << "[OK]" << std::endl;
            correct++;
        } else {
            std::cout << "[FAIL]" << std::endl;
        }
    }
    
    std::cout << "\nAccuracy: " << correct << " / " << tests.size() << std::endl;
    
    return 0;
}
