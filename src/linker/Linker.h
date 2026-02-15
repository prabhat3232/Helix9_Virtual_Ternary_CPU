#pragma once
#include <string>
#include <vector>
#include <map>
#include "../trit_word.h"

// Re-using/Redefining structures for standalone Linker
// Ideally these would be in a common header (e.g. src/common/ObjectFormat.h)
// But to keep it minimal and self-contained for now:

struct Symbol {
    std::string name;
    std::string section;
    int64_t offset;
    bool isGlobal;
    // For Linker:
    int64_t finalAddress; // Calculated during linking
};

struct Relocation {
    int64_t offset;
    std::string symbol;
    std::string type; // ABS, PCR
    std::string section; // Where to apply
};

struct Section {
    std::string name;
    int64_t baseAddress;
    std::vector<TernaryWord> data;
    // For Linker:
    int64_t size;
};

struct ObjectFile {
    std::string filename;
    std::vector<Section> sections;
    std::vector<Symbol> symbols;
    std::vector<Relocation> relocations;
};

class Linker {
public:
    Linker();
    
    // Load an object file (.ht)
    bool LoadObjectFile(const std::string& path);
    
    // Perform Linking
    bool Link();
    
    // Write Output Executable (.hx)
    bool WriteOutput(const std::string& path);

private:
    std::vector<ObjectFile> inputs;
    
    // Final Linked Data
    struct ExecutableSection {
        std::string name;
        int64_t startAddress;
        std::vector<TernaryWord> data;
    };
    std::vector<ExecutableSection> outputSections;
    
    std::map<std::string, int64_t> globalSymbolTable; // Symbol -> Absolute Address
    
    // Helpers
    void MergeSections();
    void ResolveSymbols();
    void ApplyRelocations();
};
