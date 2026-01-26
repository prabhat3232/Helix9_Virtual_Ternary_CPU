#pragma once
#include "trit_word.h"
#include "isa.h"
#include <vector>
#include <map>
#include <string>

class Assembler {
public:
    std::vector<TernaryWord> code;
    std::map<std::string, int64_t> labels;
    
    // Instruction Builders
    void LDT(int dest_reg, int64_t addr_val) {
        // Since we don't have immediate loads yet in ISA, we'll assume pure register loads for now
        // or emulate "Immediate" loading by using a data pool?
        // For PoC: effectively "LDI" if we hack the memory before running.
        // But strictly: LDT Rd, [Rs]
        // Let's implement the Encode helper.
        // For this helper, we assume the user sets up registers manually or we implement immediate opcodes later.
        // Wait, test_cpu used "hack" to load registers. 
        // Let's Stick to the ISA: LDT Rd, [Rs]
        Emit(Opcode::LDT, dest_reg, addr_val, 0); 
        // Note: addr_val here acts as Source Reg index? 
        // In test_cpu: Encode(Opcode::LDT, 0, 1, 0) -> LDT R0, [R1]
    }
    
    // We need a way to encode standard format: Op, Dest, Src1, Src2
    void Inst(int64_t opcode, int dest, int src1, int src2 = 0) {
        Emit(opcode, dest, src1, src2);
    }

    // Sugar
    void ADD(int d, int s1, int s2) { Inst(Opcode::ADD, d, s1, s2); }
    void SUB(int d, int s1, int s2) { Inst(Opcode::SUB, d, s1, s2); }
    void STT(int src, int addr_reg) { Inst(Opcode::STT, 0, src, addr_reg); } // STT Rs, [Ra]
    
    void HLT() {
        code.push_back(TernaryWord::FromInt64(Opcode::HLT));
    }

    std::vector<TernaryWord> Compile() {
        return code;
    }

private:
    void Emit(int64_t op, int dest, int s1, int s2) {
        // Op: trits 0-2 (1, 3, 9)
        // Dest: trits 3-4 (27, 81)
        // s1: trits 5-6 (243, 729)
        // s2: trits 7-8 (2187, 6561)
        int64_t val = op + (dest * 27) + (s1 * 243) + (s2 * 2187);
        code.push_back(TernaryWord::FromInt64(val));
    }
};
