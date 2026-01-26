#include "trit_word.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

// Constants for 27 trits
const int NUM_TRITS = 27;

TernaryWord TernaryWord::FromInt64(int64_t value) {
    uint64_t p = 0;
    uint64_t n = 0;
    
    // Balanced Ternary Conversion
    // Algorithm: Repeatedly divide by 3. 
    // Remainder 0 -> 0
    // Remainder 1 -> +1
    // Remainder 2 -> -1 (logic: 2 is congruent to -1 mod 3. We treat it as -1 and add 1 to the quotient to carry over)
    
    int64_t current = value;
    for (int i = 0; i < NUM_TRITS; ++i) {
        if (current == 0) break;

        int rem = current % 3;
        // Fix remainder for negative inputs in C++ (%)
        if (rem < 0) rem += 3;

        // Balanced ternary adjustment
        if (rem == 0) {
            // 0: Do nothing
            current /= 3;
        } 
        else if (rem == 1) {
            // +1: Set P bit
            p |= (1ULL << i);
            current = (current - 1) / 3;
        } 
        else if (rem == 2) {
            // -1: Set N bit
            n |= (1ULL << i);
            current = (current + 1) / 3;
        }
        
        // Handling the carry case for "2" effectively turns it into a -1 and adds 1 to upper.
        // My logic above: (current - rem)/3 for rem=1; (current - (-1))/3 for rem=-1?
        // Let's stick to standard logic:
        // rem=2 => digit=-1, carry=+1 to next pos.
        // current = (current - (-1)) / 3 = (current+1)/3. Correct.
    }
    return TernaryWord(p, n);
}

int64_t TernaryWord::ToInt64() const {
    int64_t result = 0;
    int64_t powerOf3 = 1;
    for (int i = 0; i < NUM_TRITS; ++i) {
        bool is_p = (pos >> i) & 1;
        bool is_n = (neg >> i) & 1;

        if (is_p) result += powerOf3;
        if (is_n) result -= powerOf3;
        
        powerOf3 *= 3;
    }
    return result;
}

// Kleene Logic
TernaryWord TernaryWord::Min(const TernaryWord& other) const {
    // MIN (AND): Result is MIN of inputs.
    // +1, +1 -> +1
    // +1, 0 -> 0
    // +1, -1 -> -1
    
    // Logic from research:
    // P = A.p & B.p
    // N = A.n | B.n
    return TernaryWord(pos & other.pos, neg | other.neg);
}

TernaryWord TernaryWord::Max(const TernaryWord& other) const {
    // MAX (OR): Result is MAX of inputs.
    // Logic from research:
    // P = A.p | B.p
    // N = A.n & B.n
    return TernaryWord(pos | other.pos, neg & other.neg);
}

TernaryWord TernaryWord::Negate() const {
    // Invert sign: Swap P and N
    return TernaryWord(neg, pos);
}

TernaryWord TernaryWord::Add(const TernaryWord& other) const {
    // For PoC, we implement a Ripple Carry Adder over the bits.
    // Parallel SIMD logic is possible but complex to implement correctly in one go.
    // We strictly use the ALU logic gate derivation per trit.
    
    uint64_t res_p = 0;
    uint64_t res_n = 0;
    
    // Carry Trits (Initially 0)
    int c_val = 0; // -1, 0, or 1
    
    for (int i = 0; i < NUM_TRITS; ++i) {
        // Extract A and B trits
        int a = ((pos >> i) & 1) - ((neg >> i) & 1);
        int b = ((other.pos >> i) & 1) - ((other.neg >> i) & 1);
        
        int sum = a + b + c_val;
        
        // Balanced Ternary Sum Normalization
        // Sum can range from -3 to +3 (since A,B,C are in [-1,1])
        // We want output digit in [-1,1] and new carry
        
        int digit = 0;
        int new_c = 0;
        
        if (sum > 1) {
            digit = sum - 3;
            new_c = 1;
        } else if (sum < -1) {
            digit = sum + 3;
            new_c = -1;
        } else {
            digit = sum;
            new_c = 0;
        }
        
        c_val = new_c;
        
        // Store digit
        if (digit == 1) res_p |= (1ULL << i);
        if (digit == -1) res_n |= (1ULL << i);
    }
    
    return TernaryWord(res_p, res_n);
}

std::string TernaryWord::ToString() const {
    std::string s = "";
    for (int i = NUM_TRITS - 1; i >= 0; --i) {
        bool p = (pos >> i) & 1;
        bool n = (neg >> i) & 1;
        if (p) s += "+";
        else if (n) s += "-";
        else s += "0";
    }
    return s + " (" + std::to_string(ToInt64()) + ")";
}
