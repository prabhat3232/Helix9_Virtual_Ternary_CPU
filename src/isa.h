#pragma once
#include "trit_word.h"

// Opcode Encodings (3 Trits)
// LDT: 001
// STT: 002
// ADD: 004
// SUB: 005
// MUL: 006
// MIN: 010
// MAX: 011
// NEG: 012
// JMP: 020
// JEZ: 021 (Jump Equal Zero)
// JLZ: 022 (Jump Less Zero)
// JGZ: 023 (Jump Greater Zero)

// We represent opcodes as int64_t for easy switching in C++
// Full v0.1 Opcode Set
// See docs/architecture/isa_spec.md for encoding details
enum class Opcode {
    // System (0-1)
    HLT = 0,
    NOP = 1,
    
    // Arithmetic (2-6)
    ADD = 2,
    SUB = 3,
    MUL = 4,
    DIV = 5,
    MOD = 6,
    
    // Logic (7-11)
    AND = 7,
    OR  = 8,
    XOR = 9,
    LSL = 10,
    LSR = 11,
    
    // Data Movement (12-15)
    MOV = 12, // Move Register
    LDI = 13, // Load Immediate
    LDW = 14, // Load Word
    STW = 15, // Store Word
    
    // Branch / Control (16-24)
    JMP = 16,
    BEQ = 17,
    BNE = 18,
    BGT = 19,
    BLT = 20,
    CALL = 21,
    RET = 22,
    MSR = 23,
    MRS = 24,
    
    // New
    CMP = 25,
    
    // Cognitive (Phase 6)
    CNS = 26, // Consensus
    DEC = 27, // Decay
    POP = 28, // PopCount
    SAT = 29, // Saturating Add
    
    // Vector (Phase 8 - Vector Unit)
    VEC_CNS = 30, // Vector Consensus (Legacy?)
    VEC_POP = 31, // Vector PopCount (Legacy?)
    
    // New Vector Ops
    VLDR = 32, // Load Vector
    VSTR = 33, // Store Vector
    VADD = 34, // Vector Add
    VDOT = 35, // Vector Dot Product
    VMMUL = 36, // Vector-Matrix Multiply
    VSIGN = 37, // Vector Sign (Ternary Activation)
    VCLIP = 38, // Vector Clip (Hard Tanh)
    VSTRI = 39, // Set Vector Stride
    
    UNKNOWN = 99
};

