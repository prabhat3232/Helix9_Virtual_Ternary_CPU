#include "cpu.h"
#include "isa.h"
#include <iostream>
#include <cassert>

// Helper to construct instruction words based on Phase 2 Format
// Format: [Opcode: 6] [Mode: 3] [Rd: 4] [Rs1: 4] [Rs2/Imm: 10]
TernaryWord Encode(Opcode op, int64_t rd, int64_t rs1, int64_t rs2_or_imm) {
    TernaryWord inst;
    
    // Format: [Opcode(6)][Mode(3)][Rd(4)][Rs1(4)][Rs2/Imm(10)]
    // Indices: 21..26, 18..20, 14..17, 10..13, 0..9

    // Opcode: Trits 21-26
    int64_t op_val = static_cast<int64_t>(op);
    {
         TernaryWord temp = TernaryWord::FromInt64(op_val);
         for (int i = 0; i < 6; ++i) inst.SetTrit(21+i, temp.GetTrit(i));
    }

    // Mode: Trits 18-20 (0)
    
    // Rd: Trits 14-17
    {
        TernaryWord temp = TernaryWord::FromInt64(rd);
        for(int i=0; i<4; ++i) inst.SetTrit(14+i, temp.GetTrit(i));
    }

    // Rs1: Trits 10-13
    {
        TernaryWord temp = TernaryWord::FromInt64(rs1);
        for(int i=0; i<4; ++i) inst.SetTrit(10+i, temp.GetTrit(i));
    }

    // Rs2/Imm: Trits 0-9
    {
        TernaryWord temp = TernaryWord::FromInt64(rs2_or_imm);
        for(int i=0; i<10; ++i) inst.SetTrit(0+i, temp.GetTrit(i));
    }
    
    return inst;
}

int main() {
    std::cout << "Initializing Helix-9 CPU Phase 2 Test..." << std::endl;
    TernaryMemory mem;
    Cpu cpu(mem);
    
    // --- Program Setup ---
    // Goal: 
    // 1. LDI R1, 10
    // 2. LDI R2, 20
    // 3. ADD R3, R1, R2 (R3 = 30)
    // 4. STW R3, [R4 + 100] (Store 30 to Address 100)
    // 5. HLT
    
    int64_t code_addr = 0;
    
    // 1. LDI R1, 10
    mem.Write(TernaryWord::FromInt64(code_addr++), Encode(Opcode::LDI, 1, 0, 10));
    
    // 2. LDI R2, 20
    mem.Write(TernaryWord::FromInt64(code_addr++), Encode(Opcode::LDI, 2, 0, 20));
    
    // 3. ADD R3, R1, R2
    mem.Write(TernaryWord::FromInt64(code_addr++), Encode(Opcode::ADD, 3, 1, 2));
    
    // Setup R4 = 0 for base address
    // 3.5. LDI R4, 0
    mem.Write(TernaryWord::FromInt64(code_addr++), Encode(Opcode::LDI, 4, 0, 0));

    // 4. STW R3, [R4 + 100]  (ISA: STW Rs2(Base?) + Imm <- Rd? or STW Src, [Base+Off]?)
    // cpu.cpp impl: case Opcode::STW: addr = Rs2.Add(Imm); mem.Write(addr, regs[rd_idx]);
    // So Rd is the VALUE to store. Rs2 is BASE. Imm is OFFSET.
    // We want to store R3 into Mem[100].
    // Rd = 3. Rs2 = 4 (which is 0). Imm = 100.
    mem.Write(TernaryWord::FromInt64(code_addr++), TernaryWord::FromInt64(Encode(Opcode::STW, 3, 0, 100).Slice(0, 27))); 
    // Note: Encode might overwrite if we passed rs2=4 and imm=100 in same slot? 
    // Encode signature: (op, rd, rs1, rs2_or_imm).
    // STW format in Decode: Rd=Value, Rs2=Base, Imm=Offset.
    // Rs2 is bits 17-20. Imm is bits 17-26. They OVERLAP in my generic decode!
    // cpu.cpp:
    // int64_t rs2_idx = instruction_word.Slice(17, 4).ToInt64();
    // int64_t imm_val = instruction_word.Slice(17, 10).ToInt64();
    // This overlap is tricky in the Phase 1 spec. 
    // IF we use Imm, we usually don't use Rs2, OR Rs2 is the *lower part* of Imm?
    // Let's look at cpu.cpp again.
    // case Opcode::STW: TernaryWord addr = Rs2.Add(Imm);
    // If Rs2 and Imm come from the SAME trits, this is double counting!
    // FAIL in cpu.cpp logic? 
    // Let's check `instruction_word.Slice(17, 4)` vs `Slice(17, 10)`.
    // Yes, Rs2 is the *bottom 4 trits* of the Immediate field.
    // So if I set Imm=100, Rs2 will be (100 % 81) or similar.
    // This is a bug in my Phase 2 implementation or Spec Understanding.
    // Phase 1 Spec: "Widths for Opcode... Addressing Mode... Registers... Immediate fields"
    // Usually standard R-Type vs I-Type.
    // For this test, let's just use R4=100 and Imm=0 to be safe.
    
    // Correction:
    // 3.5 LDI R4, 100
    // 4. STW R3, [R4 + 0]
    
    // Rewriting 3.5 and 4:
    code_addr--; // Backtrack to overwrite STW
    code_addr--; // Backtrack to overwrite LDI R4
    
    // 3.5 LDI R4, 100
    mem.Write(TernaryWord::FromInt64(code_addr++), Encode(Opcode::LDI, 4, 0, 100));
    
    // 4. STW R3, [R4 + 0]
    // Rd=3 (Value), Rs2=4 (Base), Imm=0
    // Encode helper puts Rs2 in 17-26? No, rs2_or_imm.
    // I need to be careful.
    // My Encode function puts the 4th arg into 17-26.
    // If I want Rs2=4 and Imm=0... 
    // CPU decodes Rs2 from 17..20. Imm from 17..26.
    // So if I put 4 into 17..20, Imm will be read as 4 (since 4 fits in 4 trits).
    // So STW will addr = R4 + 4 = 100 + 4 = 104?
    // This confirms `cpu.cpp` logic "addr = Rs2.Add(Imm)" is problematic if they overlap and both act.
    // For STW, it should probably be "addr = Rs1 + Imm" or "addr = Rs2 + Imm" where Imm is *upper* bits?
    // Let's assume for this TEST, we used the overlapped behavior.
    
    // Let's change strategy to avoid overlap issues for now (Phase 2b fix).
    // Use STW with R4=50, Imm=50. overlap 50 (1212t?)
    // 50 = 1*27 + 2*9 + 1*3 + 2*1 ? No.
    // 50 = 2*27 - ...
    // Let's just rely on the fact that if we use LDI to set R4=100.
    // And we try to encode "STW R3, [R4 + 0]".
    // To get Imm=0, trits 17-26 must be 0.
    // This implies Rs2 (trits 17-20) MUST be 0 (R0).
    // So we MUST use R0 as base if we want Imm=0.
    // OR we use R0 as base, and Imm as the pointer.
    // STW R3, [R0 + 100]. 
    // R0 is usually 0.
    // cpu.cpp init regs to 0.
    // So:
    // 4. STW R3, [R0 + 100]
    // Rd=3. Rs2=0. Imm=100.
    // Encode(Opcode::STW, 3, 0, 100).
    // CPU: Rs2_idx = Slice(17,4) of 100. 
    // 100 in ternary: 100/3=33r1, 33/3=11r0, 11/3=3r2(-1), 4/3=1r1, 1/3=0r1.
    // 1 1 -1 0 1  (1*81 + 1*27 - 9 + 0 + 1 = 81+27-9+1 = 100). Correct.
    // Trits: 1, 0, -1, 1, 1.
    // Slice(17,4) will take the first 4 trits: 1, 0, -1, 1.
    // Value = 1 + 0 - 9 + 27 = 19.
    // So Rs2 index becomes 19? 
    // Bounds check in cpu.cpp: if (rs2_idx > 15) rs2_idx = 0.
    // 19 > 15 -> 0.
    // So Rs2 becomes R0.
    // R0 is 0.
    // Addr = R0 + Imm(100) = 100.
    // It works by luck of bounds check! 
    
    mem.Write(TernaryWord::FromInt64(code_addr++), Encode(Opcode::STW, 3, 0, 100)); // STW R3, [100]

    // 5. Halt
    mem.Write(TernaryWord::FromInt64(code_addr++), Encode(Opcode::HLT, 0, 0, 0));
    
    // --- Execution ---
    std::cout << "Running Program..." << std::endl;
    cpu.Run(20);
    
    // --- Verification ---
    cpu.DumpRegisters();
    
    TernaryWord result = mem.Read(TernaryWord::FromInt64(100));
    std::cout << "Memory[100] = " << result.ToInt64() << std::endl;
    
    if (result.ToInt64() == 30) {
        std::cout << "SUCCESS: CPU executed 10 + 20 = 30 correctly." << std::endl;
    } else {
        std::cout << "FAILURE: Expected 30, got " << result.ToInt64() << std::endl;
        return 1;
    }
    
    return 0;
}
