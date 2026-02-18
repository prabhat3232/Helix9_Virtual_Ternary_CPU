#include "cpu.h"
#include <iostream>
#include <vector>



bool CheckMem(TernaryMemory& mem, int64_t addr, int64_t expected, const std::string& name) {
    int64_t val = mem.Read(TernaryWord::FromInt64(addr)).ToInt64();
    if (val == expected) {
        std::cout << "[PASS] " << name << ": Mem[" << addr << "] = " << val << std::endl;
        return true;
    } else {
        std::cout << "[FAIL] " << name << ": Mem[" << addr << "] = " << val << " (Expected " << expected << ")" << std::endl;
        return false;
    }
}

int main(int argc, char** argv) {
    std::cout << "--- HelixASM Extended Test Suite ---" << std::endl;
    
    TernaryMemory mem;
    // Load Test Suite (HTX Format)
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <executable_file.htx>" << std::endl;
        return 1;
    }
    if (!mem.LoadExecutable(argv[1])) {
        std::cerr << "Failed to load " << argv[1] << std::endl;
        return 1;
    }
    
    Cpu cpu(mem);
    cpu.Run(1000); // Run enough cycles
    
    std::cout << "--- CPU State Post-Execution ---" << std::endl;
    cpu.DumpRegisters();
    
    bool all_passed = true;
    
    // 1. Arithmetic
    all_passed &= CheckMem(mem, 201, 12, "ADD");
    all_passed &= CheckMem(mem, 202, 8,  "SUB");
    all_passed &= CheckMem(mem, 203, 20, "MUL");
    all_passed &= CheckMem(mem, 204, 5,  "DIV");
    
    // 2. Logic
    all_passed &= CheckMem(mem, 210, -1, "AND (Min)");
    all_passed &= CheckMem(mem, 211, 1,  "OR (Max)");
    all_passed &= CheckMem(mem, 212, -1, "XOR (SumMod3)");
    all_passed &= CheckMem(mem, 213, 3,  "LSL");
    all_passed &= CheckMem(mem, 214, 2,  "LSR");
    
    // 3. Memory & Directives
    all_passed &= CheckMem(mem, 220, 30, "LDW .DAT");
    
    // 4. Function Call
    all_passed &= CheckMem(mem, 230, 10, "CALL/RET");
    
    if (all_passed) {
        std::cout << "\n>>> ALL TESTS PASSED <<<\n" << std::endl;
        return 0;
    } else {
        std::cout << "\n>>> SOME TESTS FAILED <<<\n" << std::endl;
        return 1;
    }
}
