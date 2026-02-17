#include "Parser.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace Helix;
using namespace Helix::Assembler;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: helix-asm <input.hasm> [-o <output.ht>]" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = "out.ht";
    
    if (argc >= 3) {
        std::string arg2 = argv[2];
        if (arg2 == "-o" && argc >= 4) {
            outputFile = argv[3];
        } else if (arg2 != "-o") {
            outputFile = arg2;
        }
    }
    
    // Read Source
    std::ifstream in(inputFile);
    if (!in.is_open()) {
        std::cerr << "Error: Could not open input file " << inputFile << std::endl;
        return 1;
    }
    
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string source = buffer.str();
    in.close();
    
    // Compile
    try {
        Parser parser(source);
        ObjectFile obj = parser.Compile();
        
        // Write Output (.ht format)
        std::ofstream out(outputFile);
        if (!out.is_open()) {
            std::cerr << "Error: Could not open output file " << outputFile << std::endl;
            return 1;
        }
        
        // Header
        out << "HTX 1 " << obj.sections.size() << "\n";
        
        // Sections
        for (const auto& sec : obj.sections) {
            out << "SECTION " << sec.name << " " << sec.baseAddress << " " << sec.data.size() << "\n";
            for (const auto& word : sec.data) {
                out << word.ToInt64() << " ";
            }
            out << "\n";
        }
        
        // Symbols
        out << "SYMBOLS " << obj.symbols.size() << "\n";
        for (const auto& sym : obj.symbols) {
            out << sym.name << " " << sym.section << " " << sym.offset << " " << (sym.isGlobal ? "G" : "L") << "\n";
        }
        
        // Relocations
        out << "RELOCATIONS " << obj.relocations.size() << "\n";
        for (const auto& rel : obj.relocations) {
            out << rel.offset << " " << rel.symbol << " " << rel.type << " " << rel.section << "\n";
        }
        
        out.close();
        
        std::cout << "Successfully assembled to " << outputFile << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Assembler Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
