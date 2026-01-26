#include "soft_float.h"
#include "vector_alu.h"
#include <iostream>
#include <vector>

void TestNeuron() {
    std::cout << "--- Helix-9 AI Extension Test: Single Neuron ---" << std::endl;
    
    // Scenario:
    // 2 Inputs: [1.0, 2.0]
    // 2 Weights: [0.5, -0.5]
    // Bias: 0.1
    // Formula: Y = ReLU( (I1*W1 + I2*W2) + Bias )
    // Calc: (1.0*0.5 + 2.0*-0.5) + 0.1
    //     = (0.5 - 1.0) + 0.1
    //     = -0.5 + 0.1
    //     = -0.4
    // ReLU(-0.4) = 0.
    
    // Let's try positive output scenario:
    // Inputs: [3.0, 1.0]
    // Weights: [1.0, 0.5]
    // Bias: -1.0
    // Calc: (3*1 + 1*0.5) - 1
    //     = (3 + 0.5) - 1
    //     = 2.5
    // ReLU(2.5) = 2.5
    
    std::vector<TernaryFloat> inputs;
    inputs.push_back(TernaryFloat::FromDouble(3.0));
    inputs.push_back(TernaryFloat::FromDouble(1.0));
    
    std::vector<TernaryFloat> weights;
    weights.push_back(TernaryFloat::FromDouble(1.0));
    weights.push_back(TernaryFloat::FromDouble(0.5));
    
    TernaryFloat bias = TernaryFloat::FromDouble(-1.0);
    
    std::cout << "Inputs: " << inputs[0].ToString() << ", " << inputs[1].ToString() << std::endl;
    std::cout << "Weights: " << weights[0].ToString() << ", " << weights[1].ToString() << std::endl;
    std::cout << "Bias: " << bias.ToString() << std::endl;
    
    // 1. Dot Product
    TernaryFloat dot = VectorALU::DotProduct(inputs, weights);
    std::cout << "Dot Product (W*I): " << dot.ToString() << std::endl;
    
    // 2. Add Bias
    TernaryFloat pre_activation = dot.Add(bias);
    std::cout << "Pre-Activation (Dot + B): " << pre_activation.ToString() << std::endl;
    
    // 3. Activation (ReLU)
    TernaryFloat output = pre_activation.ReLU();
    std::cout << "Output (ReLU): " << output.ToString() << std::endl;
    
    // Verification
    double val = output.ToDouble();
    if (std::abs(val - 2.5) < 0.01) {
        std::cout << "SUCCESS: Neuron computed 2.5 correctly." << std::endl;
    } else {
        std::cout << "FAILURE: Expected 2.5, got " << val << std::endl;
    }
}

int main() {
    TestNeuron();
    return 0;
}
