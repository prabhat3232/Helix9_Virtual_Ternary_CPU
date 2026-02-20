#include "Linker.h"
#include <iostream>
#include <fstream>
#include <sstream>

Linker::Linker() {}

bool Linker::LoadObjectFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        std::cerr << "Error: Could not open " << path << std::endl;
        return false;
    }
    
    ObjectFile file;
    file.filename = path;
    
    std::string line;
    Section* currentSection = nullptr;
    
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string token;
        ss >> token;
        
        if (token == "HTX") {
            // Header
            int ver; ss >> ver; // 1
            // Entry point logic later
        }
        else if (token == "SECTION") {
            std::string name;
            int64_t base, size;
            ss >> name >> base >> size;
            file.sections.push_back({name, base, {}});
            currentSection = &file.sections.back();
            currentSection->size = size;
            
            // Read data line
             if (std::getline(in, line)) {
                 std::stringstream dss(line);
                 int64_t val;
                 while (dss >> val) {
                     currentSection->data.push_back(TernaryWord::FromInt64(val));
                 }
             }
        }
        else if (token == "SYMBOLS") {
            int count; ss >> count;
            for(int i=0; i<count; ++i) {
                if (!std::getline(in, line)) break;
                std::stringstream sss(line);
                std::string name, sec, scope;
                int64_t off;
                sss >> name >> sec >> off >> scope;
                
                Symbol sym;
                sym.name = name;
                sym.section = sec;
                sym.offset = off;
                sym.isGlobal = (scope == "G");
                file.symbols.push_back(sym);
            }
        }
        else if (token == "RELOCATIONS") {
            int count; ss >> count;
            for(int i=0; i<count; ++i) {
                 if (!std::getline(in, line)) break;
                 std::stringstream rss(line);
                 int64_t off;
                 std::string sym, type, sec;
                 rss >> off >> sym >> type >> sec;
                 
                 Relocation reloc;
                 reloc.offset = off;
                 reloc.symbol = sym;
                 reloc.type = type;
                 reloc.section = sec;
                 file.relocations.push_back(reloc);
            }
        }
    }
    
    inputs.push_back(file);
    return true;
}

bool Linker::Link() {
    // 1. Initialize Output Sections
    // Map Section Name -> ExecutableSection
    std::map<std::string, ExecutableSection> sectionMap;
    // Default sections
    sectionMap[".text"] = {".text", 0, {}};
    sectionMap[".data"] = {".data", 0, {}};
    
    // 2. Merge Sections & Calculate Offsets
    // We need to track where each input file's section starts in the final executable
    struct FileSectionOffset {
        std::string filename;
        std::string sectionName;
        int64_t startOffset; // Relative to merged section start
    };
    std::vector<FileSectionOffset> fileOffsets;
    
    for(const auto& file : inputs) {
        for(const auto& sec : file.sections) {
            if (sectionMap.find(sec.name) == sectionMap.end()) {
                sectionMap[sec.name] = {sec.name, 0, {}};
            }
            
            ExecutableSection& outSec = sectionMap[sec.name];
            int64_t startOffset = outSec.data.size();
            
            // Record offset for resolving symbols/relocs later
            fileOffsets.push_back({file.filename, sec.name, startOffset});
            
            // Append data
            outSec.data.insert(outSec.data.end(), sec.data.begin(), sec.data.end());
        }
    }
    
    // 3. Layout Memory (Assign base addresses)
    // .text at 0
    // .data after .text
    int64_t currentAddress = 0;
    
    // Order matters! Text first.
    if (sectionMap.count(".text")) {
        sectionMap[".text"].startAddress = currentAddress;
        currentAddress += sectionMap[".text"].data.size();
        outputSections.push_back(sectionMap[".text"]);
        // std::cout << ".text at " << sectionMap[".text"].startAddress << " size " << sectionMap[".text"].data.size() << std::endl;
    }
    
    if (sectionMap.count(".data")) {
        sectionMap[".data"].startAddress = currentAddress;
         currentAddress += sectionMap[".data"].data.size();
         outputSections.push_back(sectionMap[".data"]);
         // std::cout << ".data at " << sectionMap[".data"].startAddress << " size " << sectionMap[".data"].data.size() << std::endl;
    }
    
    // Add others?
    
    // 4. Resolve Symbols
    globalSymbolTable.clear();
    
    for(const auto& file : inputs) {
        for(const auto& sym : file.symbols) {
            if (sym.isGlobal) {
                // Find start offset of this file's section
                int64_t fileOffset = 0;
                bool foundOffset = false;
                for(const auto& fo : fileOffsets) {
                    if (fo.filename == file.filename && fo.sectionName == sym.section) {
                        fileOffset = fo.startOffset;
                        foundOffset = true;
                        break;
                    }
                }
                
                int64_t sectionBase = sectionMap[sym.section].startAddress;
                int64_t finalAddr = sectionBase + fileOffset + sym.offset;
                
                if (globalSymbolTable.count(sym.name)) {
                    std::cerr << "Error: Duplicate global symbol " << sym.name << std::endl;
                    return false;
                }
                globalSymbolTable[sym.name] = finalAddr;
            }
        }
    }
    
    // 5. Apply Relocations
    for(const auto& file : inputs) {
        for(const auto& reloc : file.relocations) {
            // Find definition
            int64_t targetAddr = 0;
            bool found = false;
            
            // Try Global
            if (globalSymbolTable.count(reloc.symbol)) {
                targetAddr = globalSymbolTable[reloc.symbol];
                found = true;
            } else {
                // Try Local (in same file)
                for(const auto& sym : file.symbols) {
                    if (sym.name == reloc.symbol) { // Match scope?
                         // Calculate addr
                         int64_t fileOffset = 0;
                         for(const auto& fo : fileOffsets) {if (fo.filename == file.filename && fo.sectionName == sym.section) { fileOffset = fo.startOffset; break; }}
                         int64_t sectionBase = sectionMap[sym.section].startAddress;
                         targetAddr = sectionBase + fileOffset + sym.offset;
                         found = true;
                         break;
                    }
                }
            }
            
            if (!found) {
                std::cerr << "Error: Undefined symbol " << reloc.symbol << std::endl;
                return false;
            }
            
            // Calculate Patch Address
            int64_t fileOffset = 0;
            for(const auto& fo : fileOffsets) {if (fo.filename == file.filename && fo.sectionName == reloc.section) { fileOffset = fo.startOffset; break; }}
            int64_t sectionBase = sectionMap[reloc.section].startAddress;
            int64_t patchAddr = sectionBase + fileOffset + reloc.offset;
            
            std::cout << "[LINK DEBUG] Relocating '" << reloc.symbol << "' at patchAddr " << patchAddr << " to targetAddr " << targetAddr << std::endl;

            // Apply
            if (reloc.type == "ABS") {
                // Patch immediate field only (lower 10 trits), preserve opcode/mode/rd
                TernaryWord original = sectionMap[reloc.section].data[fileOffset + reloc.offset];
                original.SetSlice(0, 10, patchAddr); // Wait, targetAddr not patchAddr! Reloc Value = Target Addr
                original.SetSlice(0, 10, targetAddr); 
                sectionMap[reloc.section].data[fileOffset + reloc.offset] = original;
            }
            else if (reloc.type == "PCR") {
                // Value = Target - PatchAddr
                // Wait. PC during execution is Address of NEXT instruction? Or Current?
                // Spec says usually PC = Current + 1
                // So Offset = Target - (PatchAddr + 1)
                // Let's assume standard PC-Relative behaviour.
                int64_t value = targetAddr - (patchAddr + 1);
                
                TernaryWord original = sectionMap[reloc.section].data[fileOffset + reloc.offset];
                original.SetSlice(0, 10, value);
                sectionMap[reloc.section].data[fileOffset + reloc.offset] = original;
            }
        }
    }
    
    // Update outputSections (Move this AFTER relocations)
    outputSections.clear();
    if (sectionMap.count(".text")) {
         outputSections.push_back(sectionMap[".text"]);
    }
    if (sectionMap.count(".data")) {
         outputSections.push_back(sectionMap[".data"]);
    }
    
    return true;
}

bool Linker::WriteOutput(const std::string& path) {
    std::ofstream out(path);
    if (!out.is_open()) return false;
    
    out << "HX 1 " << outputSections.size() << "\n"; // Header
    
    for(const auto& sec : outputSections) {
        out << "SECTION " << sec.name << " " << sec.startAddress << " " << sec.data.size() << "\n";
        for(const auto& word : sec.data) {
            out << word.ToInt64() << " ";
        }
        out << "\n";
    }
    
    out.close();
    return true;
}
