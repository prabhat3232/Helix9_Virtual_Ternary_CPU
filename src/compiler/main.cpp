#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: helix_c <input.c> [-o output.hasm]" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = "out.hasm";
    
    if (argc >= 4 && std::string(argv[2]) == "-o") {
        outputFile = argv[3];
    } else {
        // simple replace extension
        size_t lastindex = inputFile.find_last_of("."); 
        outputFile = inputFile.substr(0, lastindex) + ".hasm"; 
    }

    std::cout << "Compiling " << inputFile << "..." << std::endl;

    // Read Source
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << inputFile << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    // 1. Lexer
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.Tokenize();
    
    // Debug: Print Tokens
    /*
    for (const auto& t : tokens) {
        std::cout << t.ToString() << std::endl;
    }
    */

    // 2. Parser
    Parser parser(tokens);
    std::unique_ptr<Program> ast = parser.Parse();
    
    if (!ast) {
        std::cerr << "Parsing failed." << std::endl;
        return 1;
    }

    // 3. CodeGen
    CodeGen codegen(*ast);
    std::string asmCode = codegen.Generate();

    // Write Output
    std::ofstream out(outputFile);
    out << asmCode;
    out.close();

    std::cout << "Success! Output written to " << outputFile << std::endl;
    return 0;
}
