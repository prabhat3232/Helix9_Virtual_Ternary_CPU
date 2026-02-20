#include "../src/tnn/helix_runtime.h"
#include "../src/trit_word.h"
#include <iostream>
#include <vector>

using namespace Helix;

int main() {
    std::cout << "--- Helix9 TNN Runtime Test ---" << std::endl;

    // 1. Create a Synthetic Model (Small MLP)
    TNNModel model;
    model.version = 1;
    model.name = "Test_TNN_Model";

    TNNLayer dense1;
    dense1.type = "Dense";
    dense1.input_size = 2;
    dense1.output_size = 2;
    
    // Weights: [ [1, 1], [-1, -1] ]
    dense1.weights.push_back(TernaryWord::FromInt64(1));
    dense1.weights.push_back(TernaryWord::FromInt64(1));
    dense1.weights.push_back(TernaryWord::FromInt64(-1));
    dense1.weights.push_back(TernaryWord::FromInt64(-1));
    
    // Biases: [0, 0]
    dense1.biases.push_back(TernaryWord::FromInt64(0));
    dense1.biases.push_back(TernaryWord::FromInt64(0));
    
    model.layers.push_back(dense1);

    TNNLayer act1;
    act1.type = "Activation_Sign";
    act1.input_size = 2;
    act1.output_size = 2;
    model.layers.push_back(act1);

    // Save Model
    std::string filename = "test_model.htnn";
    HelixModelLoader::SaveModel(model, filename);
    std::cout << "Saved model to " << filename << std::endl;

    // 2. Load Model
    TNNModel loaded_model = HelixModelLoader::LoadModel(filename);
    std::cout << "Loaded model: " << loaded_model.name << " with " << loaded_model.layers.size() << " layers." << std::endl;

    // 3. Execute
    std::vector<TernaryWord> input = { TernaryWord::FromInt64(1), TernaryWord::FromInt64(1) };
    std::cout << "Input:  [ " << input[0].ToInt64() << " " << input[1].ToInt64() << " ]" << std::endl;

    std::vector<TernaryWord> output = HelixRuntime::Execute(loaded_model, input);

    std::cout << "Output: [ " << output[0].ToInt64() << " " << output[1].ToInt64() << " ]" << std::endl;

    // Verify Output
    // Dense1: [1*1 + 1*1] = 2. [-1*1 + -1*1] = -2.
    // Activation (Sign): Sign(2) = 1. Sign(-2) = -1.
    // Expected Output: [1, -1]
    
    bool passed = (output.size() == 2 && output[0].ToInt64() == 1 && output[1].ToInt64() == -1);

    if (passed) {
        std::cout << "SUCCESS: TNN Runtime Execution Passed!" << std::endl;
        return 0;
    } else {
        std::cout << "FAILURE: TNN Runtime output incorrect." << std::endl;
        return 1;
    }
}
