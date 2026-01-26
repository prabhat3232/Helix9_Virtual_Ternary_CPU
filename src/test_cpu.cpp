#include "cpu.h"
#include "isa.h"
#include <iostream>
#include <cassert>

// Encode helper
int64_t Encode(int64_t opcode, int64_t dest, int64_t s1, int64_t s2) {
    // Opcode: Trits 0-2 (1, 3, 9)
    // Dest: Trits 3-4 (27, 81)
    // Src1: Trits 5-6 (243, 729)
    // Src2: Trits 7-8 (2187, 6561)
    return opcode + (dest * 27) + (s1 * 243) + (s2 * 2187);
}

int main() {
    std::cout << "Initializing Helix-9 CPU..." << std::endl;
    TernaryMemory mem;
    Cpu cpu(mem);
    
    // --- Program Setup ---
    // Goal: 
    // 1. Load value 10 into R1
    // 2. Load value 20 into R2
    // 3. Add R1, R2 -> R3 (Should be 30)
    // 4. Store R3 to Memory Address 100
    // 5. Halt
    
    // But we don't have "Load Immediate" encoded yet in our simple switch case.
    // Trick: Pre-populate memory at address 50 and 51 with data, use LDT.
    
    int64_t DATA_ADDR_1 = 50;
    int64_t DATA_ADDR_2 = 51;
    int64_t OUT_ADDR = 100;
    
    mem.Write(TernaryWord::FromInt64(DATA_ADDR_1), TernaryWord::FromInt64(10));
    mem.Write(TernaryWord::FromInt64(DATA_ADDR_2), TernaryWord::FromInt64(20));
    
    // --- Writing Code at Address 0 ---
    int64_t code_addr = 0;
    
    // 0. Setup Address Registers (Since we don't have Immediate, we cheat and poke registers for addresses)
    // For a stricter test, we should have LDI (Load Immediate).
    // Let's just manually set R1=50, R2=51 for the fetch test.
    cpu.regs[1] = TernaryWord::FromInt64(DATA_ADDR_1);
    cpu.regs[2] = TernaryWord::FromInt64(DATA_ADDR_2);
    cpu.regs[4] = TernaryWord::FromInt64(OUT_ADDR); 
    
    // 1. LDT R0, [R1] (Load 10 into R0)
    mem.Write(TernaryWord::FromInt64(code_addr++), TernaryWord::FromInt64(
        Encode(Opcode::LDT, 0, 1, 0)
    ));
    
    // 2. LDT R3, [R2] (Load 20 into R3)
    mem.Write(TernaryWord::FromInt64(code_addr++), TernaryWord::FromInt64(
        Encode(Opcode::LDT, 3, 2, 0)
    ));
    
    // 3. ADD R3, R3, R0 (R3 = 20 + 10 = 30)
    mem.Write(TernaryWord::FromInt64(code_addr++), TernaryWord::FromInt64(
        Encode(Opcode::ADD, 3, 3, 0)
    ));
    
    // 4. STT R3, [R4] (Store 30 to address 100)
    mem.Write(TernaryWord::FromInt64(code_addr++), TernaryWord::FromInt64(
        Encode(Opcode::STT, 0, 3, 4) // Dest is ignored for STT in logic: "STT Rs1, [Rs2]" -> STT R3, [R4]
    ));
    
    // 5. Halt
    mem.Write(TernaryWord::FromInt64(code_addr++), TernaryWord::FromInt64(Opcode::HLT));
    
    // --- Execution ---
    std::cout << "Running Program..." << std::endl;
    cpu.Run(10);
    
    // --- Verification ---
    cpu.DumpRegisters();
    
    TernaryWord result = mem.Read(TernaryWord::FromInt64(OUT_ADDR));
    std::cout << "Memory[" << OUT_ADDR << "] = " << result.ToInt64() << std::endl;
    
    if (result.ToInt64() == 30) {
        std::cout << "SUCCESS: CPU executed 10 + 20 = 30 correctly." << std::endl;
    } else {
        std::cout << "FAILURE: Expected 30, got " << result.ToInt64() << std::endl;
        return 1;
    }
    
    return 0;
}
