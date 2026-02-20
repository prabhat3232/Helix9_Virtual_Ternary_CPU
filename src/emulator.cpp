#include <iostream>
#include <string>
#include <vector>
#include "cpu.h"
#include "memory.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: helix_emu <executable.hx> [max_cycles]" << std::endl;
        return 1;
    }

    std::string execFile = argv[1];

    int maxCycles = 50000;
    bool trace = false;
    
    for(int i=2; i<argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--trace" || arg == "-t") {
            trace = true;
        } else {
             try {
                maxCycles = std::stoi(arg);
             } catch(...) {}
        }
    }

    // Initialize System
    TernaryMemory memory;
    if (!memory.LoadExecutable(execFile)) {
        std::cerr << "Failed to load executable." << std::endl;
        return 1;
    }

    Cpu cpu(memory);
    if (trace) cpu.ToggleTrace(true);
    
    std::cout << "Starting Emulation..." << std::endl;
    
    // Execution Loop
    int cycle = 0;
    while (cycle < maxCycles && !cpu.halted) {
        cpu.Step();
        cycle++;
    }
    
    /*
    std::cout << "Emulation finished after " << cycle << " cycles." << std::endl;
    std::cout << "Final Register State:" << std::endl;
    for(int i=0; i<16; ++i) {
        std::cout << "R" << i << ": " << cpu.regs[i].ToInt64() << std::endl;
    }
    
    // Dump .DAT Section (Matrices)
    // 8192 (0x2000) is start of .DAT as per compiler
    std::cout << "\nMemory Dump (.DAT Section):" << std::endl;
    for(int i=0; i<32; ++i) { // Dump first 32 words
        int64_t addr = 8192 + i;
        int64_t val = memory.Read(TernaryWord::FromInt64(addr)).ToInt64();
        std::cout << "[" << addr << "] " << val << std::endl;
    }
    */
    
    return 0;
}
