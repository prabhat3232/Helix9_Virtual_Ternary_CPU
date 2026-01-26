#include "neural_net.h"
#include <iostream>
#include <iomanip>

#include <ctime>

int main() {
    std::srand(std::time(0)); // Seed RNG to get different results each run
    
    std::cout << "--- Helix-9 Sparse AI Learning Demo ---" << std::endl;
    std::cout << "Task: Learn XOR Function from Scratch." << std::endl;
    
    // 1. Setup Model
    // 2 Inputs -> 4 Hidden -> 1 Output
    TSAI_Model model;
    model.AddLayer(2, 8); // Increased to 8 neurons for better capacity
    model.AddLayer(8, 1);
    model.learning_rate = 0.02; // Conservative LR for stability
    
    // 2. Data
    std::vector<std::pair<std::vector<double>, std::vector<double>>> data = {
        {{0,0}, {0}},
        {{0,1}, {1}},
        {{1,0}, {1}},
        {{1,1}, {0}}
    };
    
    // 3. Training Loop
    std::cout << "Training..." << std::endl;
    for(int epoch = 0; epoch < 5000; ++epoch) {
        double total_err = 0;
        
        for(auto& sample : data) {
            std::vector<TernaryFloat> in;
            for(double d : sample.first) in.push_back(TernaryFloat::FromDouble(d));
            
            std::vector<TernaryFloat> target;
            for(double d : sample.second) target.push_back(TernaryFloat::FromDouble(d));
            
            model.Train(in, target);
        }
        model.ApplyUpdates();
        
        // Calc Error for display
        if (epoch % 500 == 0) {
            for(auto& sample : data) {
                std::vector<TernaryFloat> in;
                for(double d : sample.first) in.push_back(TernaryFloat::FromDouble(d));
                auto out = model.Forward(in);
                double y = out[0].ToDouble();
                double t = sample.second[0];
                total_err += (y-t)*(y-t);
            }
            std::cout << "Epoch " << epoch << " Loss: " << total_err << std::endl;
        }
    }
    
    // 4. Verification
    std::cout << "\n--- Final Results ---" << std::endl;
    bool pass = true;
    for(auto& sample : data) {
        std::vector<TernaryFloat> in;
        for(double d : sample.first) in.push_back(TernaryFloat::FromDouble(d));
        
        auto out = model.Forward(in);
        double y = out[0].ToDouble();
        int predicted = (y > 0.5) ? 1 : 0;
        int expected = (int)sample.second[0];
        
        std::cout << "In: " << sample.first[0] << "," << sample.first[1] 
                  << " -> Out: " << std::fixed << std::setprecision(4) << y 
                  << " [" << predicted << "] " 
                  << ((predicted == expected) ? "OK" : "FAIL") << std::endl;
                  
        if (predicted != expected) pass = false;
    }
    
    if (pass) {
        std::cout << "SUCCESS: Model Learned XOR!" << std::endl;
        model.Save("xor_brain.ternary");
    }
    else std::cout << "FAILURE: Model did not converge." << std::endl;
    
    return 0;
}
