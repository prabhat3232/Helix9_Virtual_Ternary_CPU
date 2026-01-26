#pragma once
#include "trit_word.h"
#include "memory.h"
#include <vector>

class Cpu {
public:
    // Registers
    // R0-R15: General Purpose
    // R13 = SP (Stack Pointer Convention)
    // R14 = LR (Link Register Convention)
    // R15 = PC (Program Counter Convention)
    std::vector<TernaryWord> regs;
    
    // Program Counter (Separate for clarity in PoC, or mapped to R15)
    TernaryWord pc;
    
    // Status Register
    // Trits: [0]=Zero, [1]=Negative, [2]=Overflow
    TernaryWord status;
    
    TernaryMemory& mem;
    bool halted;

public:
    Cpu(TernaryMemory& memory);
    
    void Step();
    void Run(int max_cycles = 100);
    
    // Debug
    void DumpRegisters();
};
