#include "Linker.h"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Usage: helix_ld <input1.ht> [input2.ht ...] -o <output.hx>" << std::endl;
        return 1;
    }
    
    std::vector<std::string> inputFiles;
    std::string outputFile;
    
    for(int i=1; i<argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-o") {
            if (i+1 < argc) {
                outputFile = argv[i+1];
                i++;
            } else {
                std::cerr << "Error: Missing output file after -o" << std::endl;
                return 1;
            }
        } else {
            inputFiles.push_back(arg);
        }
    }
    
    if (inputFiles.empty()) {
        std::cerr << "Error: No input files specified" << std::endl;
        return 1;
    }
    
    if (outputFile.empty()) {
        std::cerr << "Error: No output file specified (-o output.hx)" << std::endl;
        return 1;
    }
    
    Linker linker;
    
    // Load Inputs
    for(const auto& file : inputFiles) {
        if (!linker.LoadObjectFile(file)) {
            std::cerr << "Failed to load " << file << std::endl;
            return 1;
        }
    }
    
    // Link
    if (!linker.Link()) {
        std::cerr << "Linking Failed" << std::endl;
        return 1;
    }
    
    // Write Output
    if (!linker.WriteOutput(outputFile)) {
        std::cerr << "Failed to write output to " << outputFile << std::endl;
        return 1;
    }
    
    std::cout << "Successfully linked to " << outputFile << std::endl;
    return 0;
}
