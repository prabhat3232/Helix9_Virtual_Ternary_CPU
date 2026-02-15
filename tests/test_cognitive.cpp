#include "../src/trit_word.h"
#include <iostream>
#include <cassert>
#include <vector>

// Helper to print test result
void Test(const std::string& name, bool pass) {
    std::cout << "[TEST] " << name << ": " << (pass ? "PASS" : "FAIL") << std::endl;
    if (!pass) exit(1);
}

void TestPacking() {
    std::cout << "--- Testing Trit Packing ---" << std::endl;
    TernaryWord t;
    t.SetTrit(0, 1);  // +1
    t.SetTrit(1, -1); // -1
    t.SetTrit(2, 0);  // 0
    t.SetTrit(26, 1); // +1 (MSB)

    uint64_t packed = t.ToPacked();
    
    // Check 2-bit codes:
    // Trit 0 (+1) -> 01 (binary) at bits 0-1
    // Trit 1 (-1) -> 10 (binary) at bits 2-3
    // Trit 2 (0)  -> 00 (binary) at bits 4-5
    
    bool t0_ok = (packed & 3) == 1;
    bool t1_ok = ((packed >> 2) & 3) == 2;
    bool t2_ok = ((packed >> 4) & 3) == 0;
    
    Test("Packing (+1, -1, 0)", t0_ok && t1_ok && t2_ok);

    TernaryWord unpacked = TernaryWord::FromPacked(packed);
    Test("Unpacking (+1)", unpacked.GetTrit(0) == 1);
    Test("Unpacking (-1)", unpacked.GetTrit(1) == -1);
    Test("Unpacking (MSB)", unpacked.GetTrit(26) == 1);
}

void TestConsensus() {
    std::cout << "--- Testing Consensus (CNS) ---" << std::endl;
    // A: +1, -1, 0, +1
    // B: +1, -1, 0, -1
    // Expected:
    // +1, +1 -> +1 (Reinforce)
    // -1, -1 -> -1 (Reinforce)
    // 0, 0   -> 0 
    // +1, -1 -> 0  (Conflict -> Cancel)
    
    TernaryWord a, b;
    a.SetTrit(0, 1); b.SetTrit(0, 1);
    a.SetTrit(1, -1); b.SetTrit(1, -1);
    a.SetTrit(2, 0); b.SetTrit(2, 0);
    a.SetTrit(3, 1); b.SetTrit(3, -1);
    
    TernaryWord res = a.Consensus(b);
    
    Test("Reinforce +1", res.GetTrit(0) == 1);
    Test("Reinforce -1", res.GetTrit(1) == -1);
    Test("Agree 0", res.GetTrit(2) == 0);
    Test("Conflict (+1,-1->0)", res.GetTrit(3) == 0);
}

void TestDecay() {
    std::cout << "--- Testing Decay (DEC) ---" << std::endl;
    // Current: +1, -1
    // Mask: 0 (Decay to 0) - WAIT, implementation of DEC in Cpu.cpp used mask 0!
    // Let's test the generic Decay function with a mask.
    // Spec says DEC.W applies "Deterministic Decay Mask".
    // If mask has 0s, it clears. If mask has 1s (in pos/neg), it keeps?
    // Implementation: res = pos & mask.pos, neg & mask.neg
    // To KEEP a +1, mask must have +1.
    // To KEEP a -1, mask must have -1.
    // So "Keep Mask" for +1/-1 is same as the value itself?
    
    TernaryWord val;
    val.SetTrit(0, 1);
    val.SetTrit(1, -1);
    
    // Mask to CLEAR Trit 0, KEEP Trit 1
    TernaryWord mask;
    mask.SetTrit(0, 0); 
    mask.SetTrit(1, -1); // Mask must match the negative bit to keep it?
    
    TernaryWord res = val.Decay(mask);
    Test("Clear Trit 0", res.GetTrit(0) == 0);
    Test("Keep Trit 1 (-1)", res.GetTrit(1) == -1);
}

void TestPopCount() {
    std::cout << "--- Testing PopCount (POP) ---" << std::endl;
    TernaryWord t;
    t.SetTrit(0, 1);
    t.SetTrit(5, -1);
    t.SetTrit(10, 1);
    // Total 3 non-zeros
    Test("PopCount 3", t.PopCount() == 3);
}

void TestSaturatingAdd() {
    std::cout << "--- Testing Saturating Add (SAT) ---" << std::endl;
    // Max Value (all 1s)
    TernaryWord max;
    for(int i=0; i<27; ++i) max.SetTrit(i, 1);
    
    TernaryWord one; 
    one.SetTrit(0, 1);
    
    // Max + 1 should saturate to Max, not wrap/overflow to Min
    TernaryWord res = max.SaturatingAdd(one);
    
    // Check if it wrapped (carry would make it low value?) 
    // or saturated (stayed Max).
    // Our implementation returns MAX on overflow.
    Test("Saturate Max", res.GetTrit(0) == 1 && res.GetTrit(26) == 1);
    
    // Min Value (all -1s)
    TernaryWord min;
    for(int i=0; i<27; ++i) min.SetTrit(i, -1);
    
    TernaryWord neg_one;
    neg_one.SetTrit(0, -1);
    
    TernaryWord res_min = min.SaturatingAdd(neg_one);
    Test("Saturate Min", res_min.GetTrit(0) == -1 && res_min.GetTrit(26) == -1);
}


int main() {
    TestPacking();
    TestConsensus();
    TestDecay();
    TestPopCount();
    TestSaturatingAdd();
    std::cout << "All Cognitive Logic Tests Passed!" << std::endl;
    return 0;
}
