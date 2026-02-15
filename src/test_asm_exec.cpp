#include "cpu.h"
#include "isa.h"
#include <iostream>
#include <cassert>

// Encode Helper (Duplicated from test_cpu for independence)
TernaryWord Encode(Opcode op, int64_t rd, int64_t rs1, int64_t rs2_or_imm) {
    TernaryWord inst;
    int64_t op_val = static_cast<int64_t>(op);
    { TernaryWord t = TernaryWord::FromInt64(op_val); for (int i = 0; i < 6; ++i) inst.SetTrit(21+i, t.GetTrit(i)); }
    { TernaryWord t = TernaryWord::FromInt64(rd); for(int i=0; i<4; ++i) inst.SetTrit(14+i, t.GetTrit(i)); }
    { TernaryWord t = TernaryWord::FromInt64(rs1); for(int i=0; i<4; ++i) inst.SetTrit(10+i, t.GetTrit(i)); }
    { TernaryWord t = TernaryWord::FromInt64(rs2_or_imm); for(int i=0; i<10; ++i) inst.SetTrit(0+i, t.GetTrit(i)); }
    return inst;
}

int main() {
    std::cout << "--- HelixASM Execution Test (Self-Contained) ---" << std::endl;
    
    TernaryMemory mem;
    Cpu cpu(mem);
    
    // Program:
    // R1 = 10
    // R2 = 0
    // Loop:
    // ADD R2, R2, 1
    // CMP R1, R2
    // BGT Loop (R1 > R2)
    // HLT
    
    int64_t addr = 0;
    int64_t start_pc = 0;
    
    // 0: LDI R1, 10
    mem.Write(TernaryWord::FromInt64(addr++), Encode(Opcode::LDI, 1, 0, 10));
    
    // 1: LDI R2, 0 (Redundant but explicit)
    mem.Write(TernaryWord::FromInt64(addr++), Encode(Opcode::LDI, 2, 0, 0));
    
    int64_t loop_start = addr;
    
    // 2 (Loop): ADD R2, R2, 1 (Encode imm=1 into rs2_or_imm)
    // ADD format: Rd, Rs1, Rs2. 
    // Wait, ADD R2, R2, 1 -> This is R-Type or I-Type?
    // Cpu::ADD: Rd = Rs1 + Rs2.
    // If we want Immediate add, we need ADDI? (Not in v0.1).
    // Or we use LDI RTemp, 1 then ADD?
    // Let's use LDI R3, 1 first.
    
    // 2: LDI R3, 1
    mem.Write(TernaryWord::FromInt64(addr++), Encode(Opcode::LDI, 3, 0, 1));
    loop_start = addr; // Actually loop here
    
    // 3: ADD R2, R2, R3
    mem.Write(TernaryWord::FromInt64(addr++), Encode(Opcode::ADD, 2, 2, 3)); // Rs2=3
    
    // 4: CMP R1, R2 (Result flags set. Z=0, P=1 if R1>R2)
    // CMP format: Rs1, Rs2. (Rd unused or flags?).
    // Cpu::CMP: neg_rs2 = -Rs2. Res = Rs1 + neg. UpdateFlags.
    mem.Write(TernaryWord::FromInt64(addr++), Encode(Opcode::CMP, 0, 1, 2));
    
    // 5: BGT Loop (Branch if R1 > R2)
    // BGT Imm. PC = Rs1/PC + Imm.
    // We want absolute jump to loop_start (3).
    // Or PC Relative.
    // Let's use PC-Relative.
    // Current PC = 5. Target = 3.
    // Offset = 3 - 5 = -2.
    // BGT -2.
    // BGT uses Imm. Mode=0 (Rs1 Base) or Mode=4 (PC Base)?
    // Default Mode is 0. So Base=Rs1.
    // We need to set Mode=4 for PC-rel?
    // Encode helper sets Mode=0.
    // So we need to set Rs1=PC? PC is not GPR accessible as Rs1 (only R0-R11).
    // So we must use Absolute Jump via Rs1=0 (value 0) + Imm?
    // If Rs1=R0=0. Imm=3. Target=3.
    // Yes.
    mem.Write(TernaryWord::FromInt64(addr++), Encode(Opcode::BGT, 0, 0, loop_start)); 
    
    // 6: HLT
    mem.Write(TernaryWord::FromInt64(addr++), Encode(Opcode::HLT, 0, 0, 0));
    
    std::cout << "Running Loop Program..." << std::endl;
    // Trace enabled?
    cpu.Run(100);
    
    cpu.DumpRegisters();
    
    int64_t r2_val = cpu.regs[2].ToInt64();
    std::cout << "Final R2 Value: " << r2_val << std::endl;
    
    // Loop:
    // R2=0.
    // Add 1 -> R2=1. Cmp 10,1 -> +9 (P). Branch -> Loop.
    // ...
    // Add 1 -> R2=9. Cmp 10,9 -> +1 (P). Branch -> Loop.
    // Add 1 -> R2=10. Cmp 10,10 -> 0 (Z). Branch BGT (P) -> No.
    // Fallthrough HLT.
    // R2 should be 10.
    
    if (r2_val == 10) {
        std::cout << "SUCCESS: Loop executed correctly." << std::endl;
        return 0;
    } else {
        std::cout << "FAILURE: Expected R2=10, Got " << r2_val << std::endl;
        return 1;
    }
}
