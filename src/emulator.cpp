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
    int maxCycles = 1000;
    if (argc >= 3) {
        maxCycles = std::stoi(argv[2]);
    }

    // Initialize System
    TernaryMemory memory;
    if (!memory.LoadExecutable(execFile)) {
        std::cerr << "Failed to load executable." << std::endl;
        return 1;
    }

    Cpu cpu(memory);
    
    std::cout << "Starting Emulation..." << std::endl;
    
    // Execution Loop
    int cycle = 0;
    while (cycle < maxCycles && !cpu.halted) {
        cpu.Step();
        cycle++;
    }
    
    std::cout << "Emulation finished after " << cycle << " cycles." << std::endl;
    std::cout << "Final Register State:" << std::endl;
    for(int i=0; i<16; ++i) {
        std::cout << "R" << i << ": " << cpu.regs[i].ToInt64() << std::endl;
    }
    
    return 0;
}
