#include "cpu.h"
#include "isa.h"
#include <iostream>

Cpu::Cpu(TernaryMemory& memory) : mem(memory), halted(false) {
    // Initialize 16 registers
    regs.resize(16, TernaryWord::FromInt64(0));
    pc = TernaryWord::FromInt64(0);
    status = TernaryWord::FromInt64(0);
}

void Cpu::Step() {
    if (halted) return;

    // --- FETCH ---
    TernaryWord instruction_word = mem.Read(pc);
    int64_t raw_inst = instruction_word.ToInt64();
    
    // Increment PC (Sequential execution)
    pc = pc.Add(TernaryWord::FromInt64(1));

    // --- DECODE ---
    // Simple Decoding Scheme for PoC:
    // We assume the instruction word fits in an int64 for splitting.
    // Format: [Opcode: 6 trits/bits] [Dest: 4] [Src1: 4] [Src2: 4] (Conceptual)
    // Actually, let's use a simpler "Struct" approach since we don't have bit-packing macros yet.
    // We will assume the instruction value *is* the Opcode for very simple tests,
    // OR we define a fixed layout.
    
    // For this PoC level, let's say Instruction = Opcode + (Operand << 5 bits?).
    // Let's rely on C++ integer decoding for simplicity.
    // Opcode = inst % 100 (decimal representation of ternary?) No.
    // Let's treat the raw_inst as:
    // Opcode = raw_inst % 27 (First 3 trits)
    // Op1 = (raw_inst / 27) % 27
    // Op2 = (raw_inst / 729) % 27
    // Op3 = (raw_inst / 19683) ...
    
    int64_t opcode = raw_inst % 27;     // First 3 trits
    int64_t r_dest = (raw_inst / 27) % 9; // Next 2 trits (Register Index 0-8)
    int64_t r_src1 = (raw_inst / 243) % 9;
    int64_t r_src2 = (raw_inst / 2187) % 9;
    
    // --- EXECUTE ---
    switch (opcode) {
        case Opcode::HLT:
            halted = true;
            std::cout << "[CPU] Halted." << std::endl;
            break;
            
        case Opcode::LDT: // LDT Rd, [Rs1]
        {
            TernaryWord addr = regs[r_src1];
            regs[r_dest] = mem.Read(addr);
            std::cout << "[CPU] LDT R" << r_dest << " <- Mem[" << addr.ToInt64() << "] (" << regs[r_dest].ToInt64() << ")" << std::endl;
            break;
        }
            
        case Opcode::STT: // STT Rs1, [Rs2] (Store Src1 to address in Src2)
        {
            TernaryWord val = regs[r_src1];
            TernaryWord addr = regs[r_src2];
            mem.Write(addr, val);
            std::cout << "[CPU] STT R" << r_src1 << " -> Mem[" << addr.ToInt64() << "]" << std::endl;
            break;
        }

        case Opcode::ADD: // ADD Rd, Rs1, Rs2
        {
            regs[r_dest] = regs[r_src1].Add(regs[r_src2]);
            std::cout << "[CPU] ADD R" << r_dest << " = " << regs[r_src1].ToInt64() << " + " << regs[r_src2].ToInt64() << std::endl;
            break;
        }
        
        case Opcode::MIN: // MIN Rd, Rs1, Rs2
        {
            regs[r_dest] = regs[r_src1].Min(regs[r_src2]);
            std::cout << "[CPU] MIN R" << r_dest << " = " << regs[r_dest].ToInt64() << std::endl;
            break;
        }

        default:
            std::cout << "[CPU] Unknown Instruction: " << raw_inst << " Opcode: " << opcode << std::endl;
            break;
    }
}

void Cpu::Run(int max_cycles) {
    for (int i = 0; i < max_cycles && !halted; ++i) {
        Step();
    }
}

void Cpu::DumpRegisters() {
    std::cout << "--- CPU State ---" << std::endl;
    std::cout << "PC: " << pc.ToInt64() << std::endl;
    for (int i = 0; i < 4; ++i) { // Just dump first 4 for brevity
        std::cout << "R" << i << ": " << regs[i].ToInt64() << " ";
    }
    std::cout << std::endl << "-----------------" << std::endl;
}
