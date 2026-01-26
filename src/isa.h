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
// We represent opcodes as int64_t for easy switching in C++
namespace Opcode {
    // Memory
    const int64_t LDT = 1; // Load
    const int64_t STT = 2; // Store
    
    // Arithmetic
    const int64_t ADD = 3;
    const int64_t SUB = 4;
    const int64_t MUL = 5; // (Not implemented in PoC yet)
    
    // Logic
    const int64_t MIN = 6;
    const int64_t MAX = 7;
    const int64_t NEG = 8;
    
    // Control
    const int64_t JMP = 9;
    const int64_t JEZ = 10;
    const int64_t JLZ = 11;
    const int64_t JGZ = 12;
    const int64_t HLT = 0; // Halt
}
