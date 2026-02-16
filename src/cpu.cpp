#include "cpu.h"
#include "isa.h"
#include <iostream>
#include <iomanip>
#include <bitset>

Cpu::Cpu(TernaryMemory& memory) : mem(memory), halted(false), vec_unit(*this), trace_enabled(false) {
    // Registers are already array-initialized
    pc = TernaryWord::FromInt64(0);
    status = TernaryWord::FromInt64(0);
}

// Helpers
void Cpu::Trap(int64_t vector_addr) {
    // 1. Push PC (TODO: Stack implementation in Phase 2b)
    // For now, simple jump to vector
    std::cerr << "[CPU] TRAP! Vector: " << vector_addr << " at PC=" << pc.ToInt64() << std::endl;
    // pc = TernaryWord::FromInt64(vector_addr); // Real trap logic
    halted = true; // Halt for Phase 2a
}

void Cpu::UpdateFlags(const TernaryWord& result) {
    int64_t val = result.ToInt64();
    status.SetTrit(0, val == 0 ? 1 : 0); // Z
    status.SetTrit(1, val > 0 ? 1 : 0);  // P
    status.SetTrit(2, val < 0 ? 1 : 0);  // N
}

void Cpu::UpdateFlagsArithmetic(const TernaryWord& result, bool carry, bool overflow) {
    UpdateFlags(result);
    status.SetTrit(3, carry ? 1 : 0);    // C
    status.SetTrit(4, overflow ? 1 : 0); // O
}

uint64_t Cpu::Step(uint64_t max_cycles) {
    uint64_t cycles_executed = 0;
    while (!halted && cycles_executed < max_cycles) {
        // --- FETCH ---
        TernaryWord instruction_word = mem.Read(pc);
        
        // Increment PC (Sequential execution)
        pc = pc.Add(TernaryWord::FromInt64(1));

        // ... Rest of Decode/Execute ...

    // --- DECODE ---
    // Format: [Opcode(6)][Mode(3)][Rd(4)][Rs1(4)][Rs2/Imm(10)]
    // Indices: 21..26, 18..20, 14..17, 10..13, 0..9
    
    int64_t op_val = instruction_word.Slice(21, 6);
    Opcode opcode = static_cast<Opcode>(op_val);

    int64_t mode = instruction_word.Slice(18, 3);
    int64_t rd_idx = instruction_word.Slice(14, 4);
    int64_t rs1_idx = instruction_word.Slice(10, 4);
    // int64_t rs2_idx = instruction_word.Slice(0, 4); // Not strictly used if Imm
    int64_t imm_val = instruction_word.Slice(0, 10); // Rs2/Imm overlaps
    int64_t rs2_idx = imm_val; // For Reg mode, we treat lower bits as Reg index (0-15)

    // Bounds check registers
    if (rd_idx < 0 || rd_idx > 15) rd_idx = 0; // Should trap
    if (rs1_idx < 0 || rs1_idx > 15) rs1_idx = 0;
    if (rs2_idx < 0 || rs2_idx > 15) rs2_idx = 0;

    TernaryWord& Rd = regs[rd_idx];
    const TernaryWord& Rs1 = regs[rs1_idx];
    const TernaryWord& Rs2 = regs[rs2_idx];
    TernaryWord Imm = TernaryWord::FromInt64(imm_val); 
    
    // Resolve Second Operand (Register or Immediate)
    TernaryWord Op2 = (mode == 1) ? Imm : regs[rs2_idx];
    if (mode == 1) {
         // Immediate Mode
         Op2 = Imm;
    } else {
         // Register Mode (0)
         // Note: Parser uses Mode 1 for Imm. Mode 0 for Reg.
         Op2 = regs[rs2_idx];
    }

    // --- EXECUTE ---
    
    // Capture state for Flip calculation
    TernaryWord old_rd_val = regs[rd_idx];
    TernaryWord new_rd_val = old_rd_val; // Default if not modified
    bool writeback = false;

    // Trace Log
    if (trace_enabled) {
        std::cout << "[TRACE] cyc=" << metrics.total_cycles 
                  << " pc=" << pc.ToInt64() 
                  << " op=" << op_val 
                  << " R" << rd_idx << "=" << regs[rd_idx].ToString() 
                  << std::endl;
    }

    metrics.total_cycles++;
    metrics.energy_proxy++; // Base cost per instruction

    switch (opcode) {
        // System
        case Opcode::NOP: break; // Passive cycle
        case Opcode::HLT: halted = true; std::cout << "[CPU] Halted." << std::endl; break;
        case Opcode::MSR: status = Rs1; break;
        case Opcode::MRS: Rd = status; writeback = true; new_rd_val = Rd; break;

        // Arithmetic
        case Opcode::ADD: {
            metrics.active_cycles++;
            if (status.GetTrit(Cpu::BIT_COG) == 1) {
                Rd = Rs1.SaturatingAdd(Op2);
            } else {
                Rd = Rs1.Add(Op2);
            }
            UpdateFlagsArithmetic(Rd, false, false); 
            writeback = true; new_rd_val = Rd;
            break;
        }
        case Opcode::SUB: {
            metrics.active_cycles++;
            TernaryWord neg_op2 = Op2.Negate();
            Rd = Rs1.Add(neg_op2);
            UpdateFlagsArithmetic(Rd, false, false);
            writeback = true; new_rd_val = Rd;
            break;
        }
        case Opcode::MUL: {
            metrics.active_cycles++;
            int64_t res = Rs1.ToInt64() * Op2.ToInt64();
            Rd = TernaryWord::FromInt64(res);
            UpdateFlagsArithmetic(Rd, false, false); 
            writeback = true; new_rd_val = Rd;
            break;
        }
        case Opcode::DIV: {
            metrics.active_cycles++;
            int64_t d = Op2.ToInt64();
            if (d == 0) { Trap(Cpu::VECTOR_ILLEGAL); break; }
            int64_t res = Rs1.ToInt64() / d;
            Rd = TernaryWord::FromInt64(res);
            UpdateFlagsArithmetic(Rd, false, false);
            writeback = true; new_rd_val = Rd;
            break;
        }
        case Opcode::MOD: {
            metrics.active_cycles++;
            int64_t d = Op2.ToInt64();
            if (d == 0) { Trap(Cpu::VECTOR_ILLEGAL); break; }
            int64_t res = Rs1.ToInt64() % d;
            Rd = TernaryWord::FromInt64(res);
            UpdateFlags(Rd);
            writeback = true; new_rd_val = Rd;
            break;
        }

        // Logic
        case Opcode::AND: metrics.active_cycles++; Rd = Rs1.Min(Op2); UpdateFlags(Rd); writeback = true; new_rd_val = Rd; break;
        case Opcode::OR:  metrics.active_cycles++; Rd = Rs1.Max(Op2); UpdateFlags(Rd); writeback = true; new_rd_val = Rd; break;
        case Opcode::XOR: metrics.active_cycles++; Rd = Rs1.XOR(Op2); UpdateFlags(Rd); writeback = true; new_rd_val = Rd; break;
        
        case Opcode::LSL: metrics.active_cycles++; Rd = Rs1.ShiftLeft(); UpdateFlags(Rd); writeback = true; new_rd_val = Rd; break;
        case Opcode::LSR: metrics.active_cycles++; Rd = Rs1.ShiftRight(); UpdateFlags(Rd); writeback = true; new_rd_val = Rd; break;
        
        // Data
        case Opcode::LDW: {
            metrics.active_cycles++;
            TernaryWord addr = Rs1.Add(Imm);
            TernaryWord raw_addr = addr;

            // Cognitive Mode Protection (Bit 6)
            if (status.GetTrit(Cpu::BIT_COG) == 1) {
                int64_t a = addr.ToInt64();
                // 1. Validate Range (0x3000 - 0x7FFF)
                if (a < 0x3000 || a > 0x7FFF) {
                    Trap(Cpu::VECTOR_SECURE_FAULT);
                    break;
                }
                // 2. Page Wrapping (256 words)
                // A' = (A % 256) + (A & ~255)
                int64_t page_base = a & ~0xFF; 
                int64_t offset = a & 0xFF;
                // Wrapping logic handled by masking? 
                // Wait. Spec says A' = (A mod 256) + PageBase.
                // If Rs1+Imm exceeds page, it wraps.
                // But Add() already happened.
                // If the addition crossed a page boundary, we must wrap it back.
                // Example: Base=0x3000. Index=256. Res=0x3100. Should be 0x3000.
                // We need to know the 'Base' implied by the instruction?
                // Actually, Page-Local Arithmetic usually means `effective_addr = base + (index % 256)`.
                // But we computed `addr = Rs1 + Imm`.
                // If we treat `Rs1` as Base, loops work.
                // Implementation: 
                // Effective Address = (Rs1 & ~0xFF) | ((Rs1 + Imm) & 0xFF)
                // This forces the lower 8 bits to wrap within the page defined by Rs1.
                int64_t base = Rs1.ToInt64();
                int64_t effective = (base & ~0xFF) | (a & 0xFF);
                addr = TernaryWord::FromInt64(effective);
            }

            Rd = mem.Read(addr);
            writeback = true; new_rd_val = Rd;
            break;
        }
        case Opcode::STW: {
            metrics.active_cycles++;
            metrics.energy_proxy++; 
            TernaryWord addr = Rs1.Add(Imm);

             // Cognitive Mode Protection (Bit 6)
            if (status.GetTrit(Cpu::BIT_COG) == 1) {
                int64_t a = addr.ToInt64();
                if (a < 0x3000 || a > 0x7FFF) {
                    Trap(Cpu::VECTOR_SECURE_FAULT);
                    break;
                }
                // Page Wrapping
                int64_t base = Rs1.ToInt64();
                int64_t effective = (base & ~0xFF) | (a & 0xFF);
                addr = TernaryWord::FromInt64(effective);
            }

            mem.Write(addr, regs[rd_idx]); 
            break;
        }
        case Opcode::MOV: metrics.active_cycles++; Rd = Rs1; writeback = true; new_rd_val = Rd; break;
        case Opcode::LDI: metrics.active_cycles++; Rd = Imm; writeback = true; new_rd_val = Rd; break;

        // Control
        case Opcode::JMP: {
            metrics.active_cycles++;
            TernaryWord base = (mode == 4) ? pc : Rs1;
            pc = base.Add(Imm); 
            break; 
        }
        case Opcode::BEQ: {
            metrics.active_cycles++;
            if (status.GetTrit(0) == 1) {
                TernaryWord base = (mode == 4) ? pc : Rs1;
                pc = base.Add(Imm);
            }
            break; 
        }
        case Opcode::BNE: {
            metrics.active_cycles++;
            if (status.GetTrit(0) == 0) {
                TernaryWord base = (mode == 4) ? pc : Rs1;
                pc = base.Add(Imm);
            }
            break;
        }
        case Opcode::BGT: {
            metrics.active_cycles++;
            if (status.GetTrit(1) == 1) {
                TernaryWord base = (mode == 4) ? pc : Rs1;
                pc = base.Add(Imm);
            }
            break;
        }
        case Opcode::BLT: {
            metrics.active_cycles++;
            if (status.GetTrit(2) == 1) {
                TernaryWord base = (mode == 4) ? pc : Rs1;
                pc = base.Add(Imm);
            }
            break;
        }
        case Opcode::CALL: {
             metrics.active_cycles++;
             regs[14] = pc; // Store LR
             TernaryWord base = (mode == 4) ? pc : Rs1;
             pc = base.Add(Imm); 
             // Reg write to LR (R14) happened. Should calculate flips for LR too? 
             // For PoC, ignore implicit LR flips.
             break;
        }
        case Opcode::RET: metrics.active_cycles++; pc = regs[14]; break;
        
        case Opcode::CMP: {
             metrics.active_cycles++;
             TernaryWord neg_rs2 = Rs2.Negate();
             TernaryWord res = Rs1.Add(neg_rs2);
             UpdateFlagsArithmetic(res, false, false);
             break;
        }

        // Cognitive (Phase 6)
        case Opcode::CNS: {
            metrics.active_cycles++;
            Rd = Rs1.Consensus(Rs2);
            UpdateFlags(Rd);
            writeback = true; new_rd_val = Rd;
            break;
        }
        case Opcode::DEC: {
            metrics.active_cycles++;
            Rd = Rs1.Decay(Rs2); 
            UpdateFlags(Rd);
            writeback = true; new_rd_val = Rd;
            break;
        }
        case Opcode::POP: {
            metrics.active_cycles++;
            int count = Rs1.PopCount();
            Rd = TernaryWord::FromInt64(count);
            UpdateFlags(Rd);
            writeback = true; new_rd_val = Rd;
            break;
        }
        case Opcode::SAT: {
            metrics.active_cycles++;
            Rd = Rs1.SaturatingAdd(Rs2);
            UpdateFlagsArithmetic(Rd, false, false);
            writeback = true; new_rd_val = Rd;
            break;
        }

        // Vector (Phase 7)
        case Opcode::VEC_CNS:
            metrics.active_cycles += 256; 
            metrics.total_cycles += 255; // Latency Model
            vec_unit.Consensus(rd_idx, rs1_idx, rs2_idx);
            break;
        case Opcode::VEC_POP:
            metrics.active_cycles += 256;
            metrics.total_cycles += 255;
            vec_unit.PopCount(rd_idx, rs1_idx);
            break;
        case Opcode::DEC_MASK:
            metrics.active_cycles += 256;
            metrics.total_cycles += 255;
            vec_unit.DecayMask(rd_idx, rs1_idx, rs2_idx);
            break;
        case Opcode::SAT_MAC:
            metrics.active_cycles += 256;
            metrics.total_cycles += 255;
            vec_unit.SatMAC(rd_idx, rs1_idx, rs2_idx);
            break;

        default:
            std::cerr << "[CPU] Unknown Opcode: " << op_val << " at PC=" << pc.ToInt64() << std::endl;
            Trap(Cpu::VECTOR_ILLEGAL);
            break;
    }

    // Calculate Trit Flips
    if (writeback) {
        // XOR gives differences. PopCount gives magnitude of difference.
        // Wait, XOR logic: (-1,-1)->-1. (+1,+1)->+1.
        // We want changes.
        // If A != B -> flip.
        // Bitwise: packed A XOR packed B?
        // 01 (+1) vs 01 (+1) -> 00.
        // 01 (+1) vs 10 (-1) -> 11 (2 bits flip). Is this 1 trit flip or 2?
        // Conceptually 1 trit changed state.
        // 01 (+1) vs 00 (0) -> 01 (1 bit flip).
        
        // Simpler: Compare raw trits.
        // Or use Bitwise XOR of packed 2-bit?
        // metrics.trit_flips += PopCount(PackedA ^ PackedB)
        // If 01->10 (Flip +to-): 01^10 = 11 (2 bits).
        // If 00->01 (Flip 0to+): 00^01 = 01 (1 bit).
        // If 00->10 (Flip 0to-): 00^10 = 10 (1 bit).
        // So +to- counts as 2 "bit flips" but 1 "trit flip"?
        // Energy Proxy usually counts physical bit switching events.
        // So counting bit flips is physically accurate for "Energy".
        
        // Let's use XOR of Pos and Neg masks.
        uint64_t diff_p = old_rd_val.pos ^ new_rd_val.pos;
        uint64_t diff_n = old_rd_val.neg ^ new_rd_val.neg;
        int flips = std::bitset<64>(diff_p).count() + std::bitset<64>(diff_n).count();
        metrics.trit_flips += flips;
        metrics.energy_proxy += flips;
    }
    
    cycles_executed++;
    } // End While Loop
    return cycles_executed;
}


void Cpu::Run(int max_cycles) {
    uint64_t total = 0;
    while(total < (uint64_t)max_cycles && !halted) {
        total += Step(1);
    }
}

void Cpu::DumpRegisters() {
    std::cout << "--- CPU State ---" << std::endl;
    std::cout << "PC: " << pc.ToInt64() << " Status: " << status.ToString() << std::endl;
    for (int i = 0; i < 16; ++i) {
        std::cout << "R" << i << ": " << std::setw(6) << regs[i].ToInt64() << "  ";
        if ((i+1) % 4 == 0) std::cout << std::endl;
    }
    std::cout << "-----------------" << std::endl;
}

// Phase 7: Vector Unit Implementations
void Cpu::VectorUnit::Consensus(int64_t pd_idx, int64_t ps1_idx, int64_t ps2_idx) {
    int64_t pd_base = cpu.regs[pd_idx].ToInt64() & ~0xFF;
    int64_t ps1_base = cpu.regs[ps1_idx].ToInt64() & ~0xFF;
    int64_t ps2_base = cpu.regs[ps2_idx].ToInt64() & ~0xFF;

    int64_t page_pd = pd_base / 256;
    int64_t page_ps1 = ps1_base / 256;
    int64_t page_ps2 = ps2_base / 256;

    bool p1_exists = cpu.mem.IsPageAllocated(page_ps1);
    bool p2_exists = cpu.mem.IsPageAllocated(page_ps2);

    // Optimization: If both sources null, result is null (Consensus(0,0)=0).
    if (!p1_exists && !p2_exists) {
         // If Dest is allocated, we must clear it.
         // If Dest is NOT allocated, writing 0s is a no-op (handled by Write).
         // So we only need to loop if Dest exists.
         if (cpu.mem.IsPageAllocated(page_pd)) {
             for (int i = 0; i < 256; ++i) {
                 cpu.mem.Write(TernaryWord::FromInt64(pd_base + i), TernaryWord::FromInt64(0));
             }
         }
         return; 
    }

    for (int i = 0; i < 256; ++i) {
        TernaryWord s1 = cpu.mem.Read(TernaryWord::FromInt64(ps1_base + i));
        TernaryWord s2 = cpu.mem.Read(TernaryWord::FromInt64(ps2_base + i));
        TernaryWord res = s1.Consensus(s2);
        cpu.mem.Write(TernaryWord::FromInt64(pd_base + i), res);
    }
}

void Cpu::VectorUnit::PopCount(int64_t rd_idx, int64_t ps1_idx) {
     int64_t ps1_base = cpu.regs[ps1_idx].ToInt64() & ~0xFF;
     int64_t page_ps1 = ps1_base / 256;
     
     if (!cpu.mem.IsPageAllocated(page_ps1)) {
         cpu.regs[rd_idx] = TernaryWord::FromInt64(0);
         cpu.UpdateFlags(cpu.regs[rd_idx]);
         return;
     }

     int64_t total = 0;
     for (int i = 0; i < 256; ++i) {
         TernaryWord val = cpu.mem.Read(TernaryWord::FromInt64(ps1_base + i));
         total += val.PopCount();
     }
     cpu.regs[rd_idx] = TernaryWord::FromInt64(total);
     cpu.UpdateFlags(cpu.regs[rd_idx]);
}

void Cpu::VectorUnit::DecayMask(int64_t pd_idx, int64_t ps1_idx, int64_t ps2_idx) {
    int64_t pd_base = cpu.regs[pd_idx].ToInt64() & ~0xFF;
    int64_t ps1_base = cpu.regs[ps1_idx].ToInt64() & ~0xFF;
    int64_t ps2_base = cpu.regs[ps2_idx].ToInt64() & ~0xFF;
    
    int64_t page_ps1 = ps1_base / 256;
    if (!cpu.mem.IsPageAllocated(page_ps1)) {
        // Source is 0 -> Result is 0.
        int64_t page_pd = pd_base / 256;
        if (cpu.mem.IsPageAllocated(page_pd)) {
             for (int i = 0; i < 256; ++i) {
                 cpu.mem.Write(TernaryWord::FromInt64(pd_base + i), TernaryWord::FromInt64(0));
             }
        }
        return; 
    }

    for (int i = 0; i < 256; ++i) {
        TernaryWord s1 = cpu.mem.Read(TernaryWord::FromInt64(ps1_base + i));
        TernaryWord mask = cpu.mem.Read(TernaryWord::FromInt64(ps2_base + i));
        TernaryWord res = s1.Decay(mask);
        cpu.mem.Write(TernaryWord::FromInt64(pd_base + i), res);
    }
}

void Cpu::VectorUnit::SatMAC(int64_t rd_idx, int64_t ps1_idx, int64_t ps2_idx) {
    int64_t ps1_base = cpu.regs[ps1_idx].ToInt64() & ~0xFF;
    int64_t ps2_base = cpu.regs[ps2_idx].ToInt64() & ~0xFF;
    
    bool p1_exists = cpu.mem.IsPageAllocated(ps1_base / 256);
    bool p2_exists = cpu.mem.IsPageAllocated(ps2_base / 256);
    
    if (!p1_exists || !p2_exists) {
        // 0 * X = 0.
        cpu.regs[rd_idx] = TernaryWord::FromInt64(0);
        cpu.UpdateFlags(cpu.regs[rd_idx]);
        return;
    }

    int64_t acc = 0;
    for (int i = 0; i < 256; ++i) {
        int64_t v1 = cpu.mem.Read(TernaryWord::FromInt64(ps1_base + i)).ToInt64();
        int64_t v2 = cpu.mem.Read(TernaryWord::FromInt64(ps2_base + i)).ToInt64();
        acc += v1 * v2; 
    }
    cpu.regs[rd_idx] = TernaryWord::FromInt64(acc);
    cpu.UpdateFlags(cpu.regs[rd_idx]);
}
