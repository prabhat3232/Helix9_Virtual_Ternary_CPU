#include "helix_runtime.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace Helix {

TNNModel HelixModelLoader::LoadModel(const std::string& filepath) {
    TNNModel model;
    std::ifstream in(filepath);
    if (!in.is_open()) {
        std::cerr << "Error: Could not open model file " << filepath << std::endl;
        return model;
    }

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string token;
        ss >> token;

        if (token == "HTNN") {
            ss >> model.version >> model.name;
        } else if (token == "LAYER") {
            TNNLayer layer;
            ss >> layer.type >> layer.input_size >> layer.output_size;
            model.layers.push_back(layer);
        } else if (token == "WEIGHTS") {
            // Load weights for the LAST layer added
            if (model.layers.empty()) continue;
            TNNLayer& layer = model.layers.back();
            int count = layer.input_size * layer.output_size;
            for (int i = 0; i < count; ++i) {
                int64_t w;
                in >> w;
                layer.weights.push_back(TernaryWord::FromInt64(w));
            }
        } else if (token == "BIASES") {
            if (model.layers.empty()) continue;
            TNNLayer& layer = model.layers.back();
            for (int i = 0; i < layer.output_size; ++i) {
                int64_t b;
                in >> b;
                layer.biases.push_back(TernaryWord::FromInt64(b));
            }
        }
    }
    return model;
}

bool HelixModelLoader::SaveModel(const TNNModel& model, const std::string& filepath) {
    std::ofstream out(filepath);
    if (!out.is_open()) return false;

    out << "HTNN " << model.version << " " << model.name << "\n";

    for (const auto& layer : model.layers) {
        out << "LAYER " << layer.type << " " << layer.input_size << " " << layer.output_size << "\n";
        
        if (!layer.weights.empty()) {
            out << "WEIGHTS\n";
            for (const auto& w : layer.weights) {
                out << w.ToInt64() << " ";
            }
            out << "\n";
        }
        
        if (!layer.biases.empty()) {
            out << "BIASES\n";
            for (const auto& b : layer.biases) {
                out << b.ToInt64() << " ";
            }
            out << "\n";
        }
    }

    return true;
}

} // namespace Helix
