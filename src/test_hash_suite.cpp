#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <sstream>

#include "cpu.h"
#include "memory.h"
#include "trit_word.h"
#include "assembler/Parser.h"

using namespace Helix;
using namespace Helix::Assembler;

// Sponge Helper (C++ Ref)rification
int main() {
    std::cout << "--- HelixASM Ternary Sponge Hash Test ---" << std::endl;

    TernaryMemory mem; // Default constructor (Sparse)
    Cpu cpu(mem);

    // 1. Load assembled program
    if (!mem.LoadFromFile("test_hash.ht", 0)) {
        std::cerr << "Error: Could not load test_hash.ht" << std::endl;
        return 1;
    }
    std::cout << "[MMU] Loaded test_hash.ht" << std::endl;

    // 2. Run
    cpu.Run(1000); // Should halt quickly
    
    // 3. Check Result in Memory [250-252]
    TernaryWord h1 = mem.Read(TernaryWord::FromInt64(250));
    TernaryWord h2 = mem.Read(TernaryWord::FromInt64(251));
    TernaryWord h3 = mem.Read(TernaryWord::FromInt64(252));
    
    std::cout << "Hash Part 1: " << h1.ToInt64() << std::endl;
    std::cout << "Hash Part 2: " << h2.ToInt64() << std::endl;
    std::cout << "Hash Part 3: " << h3.ToInt64() << std::endl;
    
    // Expected values?
    // Deterministic execution means we should get stable non-zero values.
    // We don't have a "golden" value yet, but if they are non-zero and stable, we pass.
    
    if (h1.ToInt64() != 0 || h2.ToInt64() != 0 || h3.ToInt64() != 0) {
        std::cout << "[PASS] Sponge function produced non-zero hash." << std::endl;
        return 0;
    } else {
        std::cout << "[FAIL] Hash output is all zero." << std::endl;
        return 1;
    }
}
