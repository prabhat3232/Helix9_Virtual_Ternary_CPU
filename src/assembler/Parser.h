#pragma once
#include "Lexer.h"
#include "../trit_word.h"
#include <map>
#include <vector>
#include <string>

namespace Helix {
namespace Assembler {

enum class OperandType {
    REGISTER,
    IMMEDIATE, // Number or Label Address
    MEMORY     // [Reg] or [Reg + Imm]
};

struct Operand {
    OperandType type;
    int64_t reg; // Register index (0-15)
    int64_t imm; // Immediate value or Offset
    std::string symbol; // Label name if applicable
};

struct Section {
    std::string name;
    int64_t baseAddress;
    std::vector<TernaryWord> data;
};

struct Symbol {
    std::string name;
    std::string section;
    int64_t offset;
    bool isGlobal;
};

struct Relocation {
    int64_t offset;
    std::string symbol;
    std::string type; // ABS, PCR
    std::string section; // Where to apply
};

struct ObjectFile {
    std::vector<Section> sections;
    std::vector<Symbol> symbols;
    std::vector<Relocation> relocations;
};

} // namespace Assembler
} // namespace Helix

namespace Helix {
namespace Assembler {

class Parser {
public:
    Parser(const std::string& source);
    ObjectFile Compile(); 

private:
    std::string src;
    std::map<std::string, int64_t> symbolTable; // Local fast lookups
    
    ObjectFile result;
    Section* currentSection; // Pointer to current section in result

    
    // Lexer state for passes
    int pos;

    // Pass 1: Scan Labels
    void Pass1();
    
    // Pass 2: Generate Code
    void Pass2();

    // Helpers
    std::vector<Operand> ParseOperands(Lexer& lexer, bool dryRun);
    TernaryWord EncodeInstruction(const std::string& mnemonic, const std::vector<Operand>& ops);
    
    int64_t currentAddress;
};

} // namespace Assembler
} // namespace Helix
