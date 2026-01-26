#include "soft_float.h"
#include "vector_alu.h"
#include <iostream>
#include <vector>

// Simple Neural Layer
class DenseLayer {
public:
    int input_size;
    int output_size;
    std::vector<std::vector<TernaryFloat>> weights; // [Output][Input]
    std::vector<TernaryFloat> biases; // [Output]

    DenseLayer(int in_size, int out_size) : input_size(in_size), output_size(out_size) {
        // Initialize weights (normally random, here hardcoded/zero for demo)
        weights.resize(out_size);
        for(int i=0; i<out_size; ++i) {
            weights[i].resize(in_size, TernaryFloat(0,0));
        }
        biases.resize(out_size, TernaryFloat(0,0));
    }
    
    // Set a specific neuron's weights
    void SetWeights(int neuron_idx, const std::vector<double>& w, double b) {
        if (neuron_idx >= output_size) return;
        biases[neuron_idx] = TernaryFloat::FromDouble(b);
        for(size_t i=0; i<w.size() && i<weights[neuron_idx].size(); ++i) {
            weights[neuron_idx][i] = TernaryFloat::FromDouble(w[i]);
        }
    }

    std::vector<TernaryFloat> Forward(const std::vector<TernaryFloat>& inputs) {
        std::vector<TernaryFloat> outputs;
        for(int i=0; i<output_size; ++i) {
            // Y = Dot(W, I) + B
            TernaryFloat dot = VectorALU::DotProduct(inputs, weights[i]);
            TernaryFloat z = dot.Add(biases[i]);
            // Relu Activation
            outputs.push_back(z.ReLU());
        }
        return outputs;
    }
};

void DemoXOR() {
    std::cout << "--- Helix-9 MLP Demo: Solving Logic Gate ---" << std::endl;
    // XOR Problem is non-linear, needs 2 layers.
    // Inputs: x1, x2
    // Hidden Layer: 2 Neurons
    // Output Layer: 1 Neuron
    
    // XOR Truth Table
    // 0, 0 -> 0
    // 0, 1 -> 1
    // 1, 0 -> 1
    // 1, 1 -> 0
    
    // Network Config
    DenseLayer hidden(2, 2);
    DenseLayer output(2, 1);
    
    // Hardcoded Weights for XOR (Standard Solution)
    // H1 (OR like): W=[1, 1], B=0 -> ReLU(x1+x2)
    // H2 (NAND like, requires shifting): 
    // Let's use simpler logic: H1 = x1+x2, H2 = -x1-x2? No.
    // Classic: H1 = NAND(x1, x2), H2 = OR(x1, x2), Out = AND(H1, H2).
    
    // Let's try:
    // H1: W=[1, 1], B=-0.5 (Activated if sum >= 1) -> OR Gate
    // H2: W=[-1, -1], B=1.5 (Activated if sum <= 1) -> NAND Gate
    // Out: W=[1, 1], B=-1.5 (Activated if H1=1 AND H2=1) -> AND Gate
    
    // Correct Logic for ReLU(0.0) being 0.0:
    // H1 (OR): W=[1, 1], B=-0.5. (0,0)->-0.5->0. (0,1)->0.5->0.5. (1,1)->1.5->1.5. Output range [0, 0.5, 1.5]
    // H2 (AND): W=[1, 1], B=-1.5. (0,0)->-1.5->0. (0,1)->-0.5->0. (1,1)->0.5->0.5. Output range [0, 0, 0.5]
    // Out: W=[1, -2], B=0.
    // XOR = OR - AND? 
    // (0,0): H1=0, H2=0 -> 0.
    // (0,1): H1=0.5, H2=0 -> 0.5 - 0 = 0.5.
    // (1,0): H1=0.5, H2=0 -> 0.5 - 0 = 0.5.
    // (1,1): H1=1.5, H2=0.5 -> 1.5 - (2*0.5) = 1.5 - 1.0 = 0.5.
    
    // Refining for strict XOR:
    // H1 (OR): W=[2, 2], B=-1. (0,0)->-1->0. (0,1)->1->1. (1,1)->3->3.
    // H2 (AND): W=[2, 2], B=-3. (0,0)->-3->0. (0,1)->-1->0. (1,1)->1->1.
    // Out (Diff): W=[1, -2], B=0. 
    // (0,0): 0 - 0 = 0.
    // (0,1): 1 - 0 = 1.
    // (1,1): 3 - 2 = 1. <-- Wait, 3 - 2 = 1. This is the issue. 
    // H1 output for (1,1) is 3. We want H1 to be saturated at 1?
    // TReLU doesn't saturate.
    
    // New Strategy:
    // Make H1 a pure OR that outputs ~1 for both (0,1) and (1,1).
    // H1: W=[10, 10], B=-5? No, that just scales.
    
    // Let's use:
    // H1 (NAND-ish): W=[-1, -1], B=1.5. (0,0)->1.5. (0,1)->0.5. (1,1)->-0.5->0.
    // H2 (OR-ish):   W=[1, 1], B=-0.5. (0,0)->0. (0,1)->0.5. (1,1)->1.5.
    // Out (AND): W=[1, 1], B=-1.5? No.
    
    // Valid XOR construction with ReLU:
    // N1 = ReLU(x1 - x2)  --> (1 if 1,0 else 0)
    // N2 = ReLU(x2 - x1)  --> (1 if 0,1 else 0)
    // Out = ReLU(N1 + N2) --> (1 if either, 0 if both 0 or both 1)
    
    hidden.SetWeights(0, {1.0, -1.0}, 0.0); // x1 - x2
    hidden.SetWeights(1, {-1.0, 1.0}, 0.0); // x2 - x1
    
    output.SetWeights(0, {1.0, 1.0}, 0.0); // Sum
    
    // Check:
    // 0,0 -> N1=0, N2=0 -> Out=0
    // 0,1 -> N1=ReLU(-1)=0, N2=ReLU(1)=1 -> Out=1
    // 1,0 -> N1=ReLU(1)=1, N2=ReLU(-1)=0 -> Out=1
    // 1,1 -> N1=0, N2=0 -> Out=0
    // PERFECT!
    
    // Test Data
    std::vector<std::pair<double, double>> tests = {
        {0, 0},
        {0, 1},
        {1, 0},
        {1, 1}
    };
    
    std::cout << "Running XOR Test..." << std::endl;
    
    for(auto t : tests) {
        std::vector<TernaryFloat> ins;
        ins.push_back(TernaryFloat::FromDouble(t.first));
        ins.push_back(TernaryFloat::FromDouble(t.second));
        
        // Forward
        auto h_out = hidden.Forward(ins);
        auto final = output.Forward(h_out);
        
        double res = final[0].ToDouble();
        std::cout << "Input: " << t.first << ", " << t.second 
                  << " -> Output: " << res 
                  << " (Expected: " << ((int)t.first ^ (int)t.second) << ")" << std::endl;
    }
}

int main() {
    DemoXOR();
    return 0;
}
