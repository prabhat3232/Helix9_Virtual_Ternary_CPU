#pragma once
#include "trit_word.h"
#include "memory.h"
#include <vector>

class Cpu {
public:
    // Registers (v0.1 Spec: 16 GPRs)
    // R0-R11: General
    // R12 = FP
    // R13 = SP
    // R14 = LR
    // R15 = PC
    TernaryWord regs[16];
    
    // Program Counter
    TernaryWord pc;
    
    // Status Register
    // [0]=Z, [1]=P, [2]=N, [3]=C, [4]=O, [5]=IM
    // [0]=Z, [1]=P, [2]=N, [3]=C, [4]=O, [5]=IM, [6]=COG
    TernaryWord status; // COG=Cognitive Mode
    static const int BIT_COG = 6;

    
    TernaryMemory& mem;
    bool halted;

    // Interrupt Vector Table offsets (v0.1)
    static const int64_t VECTOR_RESET = 0x0000;
    static const int64_t VECTOR_ILLEGAL = 0x0001;
    static const int64_t VECTOR_SECURE_FAULT = 0x0002;

    // Phase 6: Observability
    struct Metrics {
        uint64_t total_cycles = 0;
        uint64_t active_cycles = 0; // Non-NOP / Useful work
        uint64_t energy_proxy = 0;  // Ins + Mem + Flips
        uint64_t trit_flips = 0;
    } metrics;

    bool trace_enabled = false;

public:
    Cpu(TernaryMemory& memory);
    
    uint64_t Step(uint64_t max_cycles = 1);
    void Run(int max_cycles = 100);
    
    // Helpers
    void Trap(int64_t vector_addr);
    void UpdateFlags(const TernaryWord& result);
    void UpdateFlagsArithmetic(const TernaryWord& result, bool carry, bool overflow);

    // Debug
    void DumpRegisters();
    void ToggleTrace(bool enable);
    void DumpMetrics();

    // Phase 7: Vector Unit
    struct VectorUnit {
        Cpu& cpu;
        VectorUnit(Cpu& c) : cpu(c) {}
        
        // Operations (Atomic Page Level)
        void Consensus(int64_t pd_idx, int64_t ps1_idx, int64_t ps2_idx);
        void PopCount(int64_t rd_idx, int64_t ps1_idx);
        void DecayMask(int64_t pd_idx, int64_t ps1_idx, int64_t ps2_idx);
        void SatMAC(int64_t rd_idx, int64_t ps1_idx, int64_t ps2_idx);
    } vec_unit;
};

