#include "cpu.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "--- HelixASM Execution Test ---" << std::endl;
    
    TernaryMemory mem;
    if (!mem.LoadFromFile("test.ht", 0)) {
        std::cerr << "Failed to load test.ht" << std::endl;
        return 1;
    }
    
    Cpu cpu(mem);
    
    std::cout << "Running loaded program..." << std::endl;
    cpu.Run(100);
    
    cpu.DumpRegisters();
    
    // Check results based on test_program.hasm
    // We expect R2 to be > R1 (10).
    // Loop: R2 starts at 0. R1=10.
    // R2 += 1. 
    // SUB R4, R1(10), R2(1) = 9. BGT Loop.
    // ...
    // When R2=10: SUB 10-10=0. BGT Loop (Fail). Fallthrough.
    // So R2 should be 10.
    
    int64_t r2_val = cpu.regs[2].ToInt64();
    std::cout << "Final R2 Value: " << r2_val << std::endl;
    
    if (r2_val == 10) {
        std::cout << "SUCCESS: Loop executed correctly." << std::endl;
        return 0;
    } else {
        std::cout << "FAILURE: Expected R2=10" << std::endl;
        return 1;
    }
}
