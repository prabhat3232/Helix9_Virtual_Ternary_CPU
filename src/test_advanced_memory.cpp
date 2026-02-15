#include "memory.h"
#include "trit_word.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <random>

// Advanced Testing for Sparse Memory & Cognitive Trace
// 1. Stress Test Allocation
// 2. Test Sparsity Optimization
// 3. Test Boundary Conditions

void Assert(bool cond, const std::string& msg) {
    if (!cond) {
        std::cerr << "FAIL: " << msg << std::endl;
        exit(1);
    } else {
        // std::cout << "PASS: " << msg << std::endl;
    }
}

int main() {
    std::cout << "--- Advanced Memory & Debug Test ---" << std::endl;
    TernaryMemory mem;

    // --- Test 1: Sparse Allocation Stress ---
    std::cout << "[Test] Allocating 1000 Random Pages..." << std::endl;
    for(int i=0; i<1000; ++i) {
        int64_t page = 0x3000/256 + (i * 2); // Skip every other page
        int64_t addr = page * 256 + 5;
        // Write non-zero to force allocation (Writing 0 optimizes to no-op)
        mem.Write(TernaryWord::FromInt64(addr), TernaryWord::FromInt64(i + 1));
        Assert(mem.IsPageAllocated(page), "Page allocated on write");
        Assert(!mem.IsPageAllocated(page+1), "Skipped page is unallocated");
    }
    
    // Check values back
    for(int i=0; i<1000; ++i) {
        int64_t page = 0x3000/256 + (i * 2);
        int64_t addr = page * 256 + 5;
        TernaryWord val = mem.Read(TernaryWord::FromInt64(addr));
        Assert(val.ToInt64() == i + 1, "Value readback correct");
    }
    std::cout << "PASS: Random page Access." << std::endl;

    // --- Test 2: OptimizePage (Deallocation) ---
    std::cout << "[Test] Optimization (Deallocation)..." << std::endl;
    int64_t test_page = 3000; // Stress test goes up to 2046. 3000 is safe.
    int64_t test_addr = test_page * 256 + 10;
    
    // Allocate
    mem.Write(TernaryWord::FromInt64(test_addr), TernaryWord::FromInt64(42));
    Assert(mem.IsPageAllocated(test_page), "Test page allocated");
    
    // Clear
    mem.Write(TernaryWord::FromInt64(test_addr), TernaryWord::FromInt64(0));
    // Verify it's still allocated (Write doesn't auto-optimize)
    Assert(mem.IsPageAllocated(test_page), "Page persists after zeroing");
    
    // Run Optimize
    mem.OptimizePage(test_page);
    Assert(!mem.IsPageAllocated(test_page), "Page deallocated after Optimize");
    
    // Verify Read is 0
    TernaryWord val = mem.Read(TernaryWord::FromInt64(test_addr));
    Assert(val.ToInt64() == 0, "Read unallocated returns 0");
    std::cout << "PASS: OptimizePage." << std::endl;

    // --- Test 3: System Memory vs Cognitive Memory ---
    std::cout << "[Test] System vs Cognitive Boundaries..." << std::endl;
    mem.Write(TernaryWord::FromInt64(0), TernaryWord::FromInt64(123)); // System
    mem.Write(TernaryWord::FromInt64(0x3000), TernaryWord::FromInt64(456)); // Cognitive
    
    Assert(mem.Read(TernaryWord::FromInt64(0)).ToInt64() == 123, "System read");
    Assert(mem.Read(TernaryWord::FromInt64(0x3000)).ToInt64() == 456, "Cognitive read");
    
    std::cout << "PASS: Memory Boundaries." << std::endl;
    
    std::cout << "--- Advanced Tests Complete ---" << std::endl;
    return 0;

}
