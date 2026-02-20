#include "../src/cpu.h"
#include "../src/isa.h"
#include "../src/trit_word.h"
#include <iostream>
#include <vector>

// TNN Benchmark: Scalar vs Vector Dot Product
// Goal: Demonstrate speedup of Dimension=32 Dot Product

// Helper to encode instruction
TernaryWord Encode(int opcode, int mode, int rd, int rs1, int rs2_imm) {
    TernaryWord word;
    word.SetSlice(21, 6, opcode);
    word.SetSlice(18, 3, mode);
    word.SetSlice(14, 4, rd);
    word.SetSlice(10, 4, rs1);
    word.SetSlice(0, 10, rs2_imm);
    return word;
}

int main() {
    TernaryMemory mem; 
    Cpu cpu(mem);
    
    // Setup Vectors
    int64_t addr_v1 = 100;
    int64_t addr_v2 = 200;
    int vec_len = 32;
    
    // Init Vectors with +1
    for(int i=0; i<vec_len; ++i) {
        mem.Write(TernaryWord::FromInt64(addr_v1 + i), TernaryWord::FromInt64(1));
        mem.Write(TernaryWord::FromInt64(addr_v2 + i), TernaryWord::FromInt64(1));
    }
    
    std::cout << "--- TNN Benchmark: Dot Product (Dim=" << vec_len << ") ---" << std::endl;

    // ------------------------------------------
    // 1. Scalar Implementation
    // ------------------------------------------
    uint64_t start_addr_scalar = 0;
    cpu.pc = TernaryWord::FromInt64(start_addr_scalar);
    
    std::vector<TernaryWord> scalar_code = {
        // Init
        Encode((int)Opcode::LDI, 1, 1, 0, 0),       // LDI R1, 0 (Acc)
        Encode((int)Opcode::LDI, 1, 2, 0, 0),       // LDI R2, 0 (i)
        Encode((int)Opcode::LDI, 1, 3, 0, vec_len), // LDI R3, 32 (Limit)
        
        // Loop Start (Addr: 3)
        // Check Limit
        Encode((int)Opcode::CMP, 0, 0, 2, 3),       // CMP R2, R3 (Mode 0: Regs)
        Encode((int)Opcode::BEQ, 1, 0, 0, 18),      // BEQ 18 (to End HLT)
        
        // Load V1[i]
        Encode((int)Opcode::LDI, 1, 4, 0, addr_v1), // LDI R4, Base1
        Encode((int)Opcode::ADD, 0, 4, 4, 2),       // ADD R4, R4, R2
        Encode((int)Opcode::LDW, 2, 6, 4, 0),       // LDW R6, [R4]
        
        // Load V2[i]
        Encode((int)Opcode::LDI, 1, 5, 0, addr_v2), // LDI R5, Base2
        Encode((int)Opcode::ADD, 0, 5, 5, 2),       // ADD R5, R5, R2
        Encode((int)Opcode::LDW, 2, 7, 5, 0),       // LDW R7, [R5]
        
        // Multiply
        Encode((int)Opcode::MUL, 0, 6, 6, 7),       // MUL R6, R6, R7
        
        // Accumulate
        Encode((int)Opcode::ADD, 0, 1, 1, 6),       // ADD R1, R1, R6
        
        // Increment i
        Encode((int)Opcode::LDI, 1, 8, 0, 1),       // LDI R8, 1
        Encode((int)Opcode::ADD, 0, 2, 2, 8),       // ADD R2, R2, R8
        
        Encode((int)Opcode::JMP, 4, 0, 0, -14),     // JMP -14 (PC Relative)
        
        // End
        Encode((int)Opcode::HLT, 0, 0, 0, 0)
    };
    
    for(size_t i=0; i<scalar_code.size(); ++i) {
        mem.Write(TernaryWord::FromInt64(start_addr_scalar + i), scalar_code[i]);
    }

    // DEBUG: Verify Memory (Scalar Start)
    // TernaryWord check = mem.Read(TernaryWord::FromInt64(0));
    // std::cout << "[DEBUG] Mem[0] Opcode Slice (21,6): " << check.Slice(21, 6) << std::endl;
    
    // Run Scalar
    cpu.metrics.active_cycles = 0; // Reset metrics
    cpu.halted = false;
    cpu.Run(10000);
    
    std::cout << "[Scalar] Active Cycles: " << cpu.metrics.active_cycles << std::endl;
    std::cout << "[Scalar] Result: " << cpu.regs[1].ToInt64() << " (Expected 32)" << std::endl;
    
    uint64_t scalar_cycles = cpu.metrics.active_cycles > 0 ? cpu.metrics.active_cycles : 1;

    // ------------------------------------------
    // 2. Vector Implementation
    // ------------------------------------------
    uint64_t start_addr_vector = 1000;
    cpu.pc = TernaryWord::FromInt64(start_addr_vector);
    
    std::vector<TernaryWord> vector_code = {
        Encode((int)Opcode::LDI, 1, 1, 0, addr_v1), // LDI R1, Base1
        Encode((int)Opcode::LDI, 1, 2, 0, addr_v2), // LDI R2, Base2
        
        // VLDR V0, R1 (Rd=0->V0, Rs1=1->R1)
        Encode((int)Opcode::VLDR, 0, 0, 1, 0),
        
        // VLDR V1, R2
        Encode((int)Opcode::VLDR, 0, 1, 2, 0),
        
        // VDOT R3, V0, V1 (Rd=3, Rs1=0->V0, Rs2=1->V1)
        // VDOT uses Rs2 field for the second vector index logic in CPU
        Encode((int)Opcode::VDOT, 0, 3, 0, 1),
        
        Encode((int)Opcode::HLT, 0, 0, 0, 0)
    };
    
    for(size_t i=0; i<vector_code.size(); ++i) {
        mem.Write(TernaryWord::FromInt64(start_addr_vector + i), vector_code[i]);
    }
    
    // Run Vector
    cpu.metrics.active_cycles = 0;
    cpu.halted = false;
    cpu.Run(10000);
    
    std::cout << "[Vector] Active Cycles: " << cpu.metrics.active_cycles << std::endl;
    std::cout << "[Vector] Result: " << cpu.regs[3].ToInt64() << std::endl; // Result in R3
    
    uint64_t vector_cycles = cpu.metrics.active_cycles > 0 ? cpu.metrics.active_cycles : 1;
    
    std::cout << "--- Results ---" << std::endl;
    std::cout << "Speedup: " << (double)scalar_cycles / vector_cycles << "x" << std::endl;
    
    // ------------------------------------------
    // 3. Fusion Benchmark: VMMUL + VSIGN vs VMMSGN
    // ------------------------------------------
    std::cout << "\n--- Fusion Optimization Benchmark ---" << std::endl;
    int64_t matrix_base = 2000;
    // Init a 32x32 matrix with 1s and -1s
    for(int i=0; i<vec_len*vec_len; ++i) {
        mem.Write(TernaryWord::FromInt64(matrix_base + i), TernaryWord::FromInt64( (i%2==0) ? 1 : -1 ));
    }
    
    // Unfused Code
    uint64_t start_addr_unfused = 20000;
    cpu.pc = TernaryWord::FromInt64(start_addr_unfused);
    std::vector<TernaryWord> unfused_code = {
        Encode((int)Opcode::LDI, 1, 1, 0, addr_v1), // LDI R1, Base1 (Input Vec)
        Encode((int)Opcode::LDI, 1, 2, 0, matrix_base), // LDI R2, Matrix Base
        
        Encode((int)Opcode::VLDR, 0, 0, 1, 0),      // VLDR V0, R1
        
        Encode((int)Opcode::VMMUL, 0, 1, 0, 2),     // VMMUL V1, V0, R2
        Encode((int)Opcode::VSIGN, 0, 1, 1, 0),     // VSIGN V1, V1
        
        Encode((int)Opcode::HLT, 0, 0, 0, 0)
    };
    for(size_t i=0; i<unfused_code.size(); ++i) mem.Write(TernaryWord::FromInt64(start_addr_unfused + i), unfused_code[i]);
    
    cpu.metrics.active_cycles = 0; cpu.halted = false;
    cpu.Run(100);
    uint64_t unfused_cycles = cpu.metrics.active_cycles;
    std::cout << "[Unfused: VMMUL + VSIGN] Active Cycles: " << unfused_cycles << std::endl;
    
    // Fused Code
    uint64_t start_addr_fused = 30000;
    cpu.pc = TernaryWord::FromInt64(start_addr_fused);
    std::vector<TernaryWord> fused_code = {
        Encode((int)Opcode::LDI, 1, 1, 0, addr_v1),
        Encode((int)Opcode::LDI, 1, 2, 0, matrix_base),
        
        Encode((int)Opcode::VLDR, 0, 0, 1, 0),      // VLDR V0, R1
        
        Encode((int)Opcode::VMMSGN, 0, 1, 0, 2),    // VMMSGN V1, V0, R2
        
        Encode((int)Opcode::HLT, 0, 0, 0, 0)
    };
    for(size_t i=0; i<fused_code.size(); ++i) mem.Write(TernaryWord::FromInt64(start_addr_fused + i), fused_code[i]);
    
    cpu.metrics.active_cycles = 0; cpu.halted = false;
    cpu.Run(100);
    uint64_t fused_cycles = cpu.metrics.active_cycles;
    
    std::cout << "[Fused: VMMSGN] Active Cycles: " << fused_cycles << std::endl;
    std::cout << "Cycle Savings: " << (unfused_cycles - fused_cycles) << " cycles per layer." << std::endl;
    
    return 0;
}
