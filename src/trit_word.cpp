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

int8_t TernaryWord::GetTrit(int index) const {
    if (index < 0 || index >= NUM_TRITS) return 0;
    bool p = (pos >> index) & 1;
    bool n = (neg >> index) & 1;
    if (p) return 1;
    if (n) return -1;
    return 0;
}

void TernaryWord::SetTrit(int index, int8_t value) {
    if (index < 0 || index >= NUM_TRITS) return;
    
    // Clear existing
    pos &= ~(1ULL << index);
    neg &= ~(1ULL << index);
    
    // Set new
    if (value == 1) pos |= (1ULL << index);
    else if (value == -1) neg |= (1ULL << index);
}

int64_t TernaryWord::Slice(int start, int len) const {
    int64_t val = 0;
    int64_t power = 1;
    for (int i = 0; i < len; ++i) {
        if (start + i < 27) {
            val += GetTrit(start + i) * power;
            power *= 3;
        }
    }
    return val;
}

void TernaryWord::SetSlice(int start, int len, int64_t val) {
    int64_t remainder = val;
    for (int i = 0; i < len; ++i) {
        if (start + i >= NUM_TRITS) break;
        
        int rem = remainder % 3;
        // Fix remainder for negative inputs
        if (rem < 0) rem += 3;
        
        int8_t t = 0;
        
        if (rem == 0) {
            t = 0;
            remainder /= 3;
        } else if (rem == 1) {
            t = 1;
            remainder = (remainder - 1) / 3;
        } else if (rem == 2) {
            t = -1;
            remainder = (remainder + 1) / 3; 
        }
        
        SetTrit(start + i, t);
    }
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

// Logic: Consensus / Sum Mod 3
TernaryWord TernaryWord::XOR(const TernaryWord& other) const {
    TernaryWord result;
    for (int i = 0; i < NUM_TRITS; ++i) {
        int a = GetTrit(i);
        int b = other.GetTrit(i);
        
        // Balanced Ternary Sum Mod 3 (Consensus)
        int sum = a + b;
        if (sum > 1) { // 2 -> -1
             sum = -1;
        } else if (sum < -1) { // -2 -> 1
             sum = 1;
        }
        
        result.SetTrit(i, sum);
    }
    return result;
}

// Logic: Shift Left (Zero fill)
TernaryWord TernaryWord::ShiftLeft() const {
    TernaryWord result;
    // T[i] = T[i-1], T[0] = 0
    for (int i = 1; i < NUM_TRITS; ++i) {
        result.SetTrit(i, GetTrit(i - 1));
    }
    result.SetTrit(0, 0);
    return result;
}

// Logic: Shift Right (Zero fill)
TernaryWord TernaryWord::ShiftRight() const {
    TernaryWord result;
    // T[i] = T[i+1], T[26] = 0
    for (int i = 0; i < NUM_TRITS - 1; ++i) {
        result.SetTrit(i, GetTrit(i + 1));
    }
    result.SetTrit(NUM_TRITS - 1, 0);
    return result;
}

// --- Cognitive Operations (Phase 6) ---

// Consensus: A=B -> A, else 0
// Bitwise AND of Pos and Neg masks handles this natively in balanced ternary.
// (+1, +1) -> +1 (pos&pos, neg&neg = 1,0)
// (-1, -1) -> -1 (pos&pos, neg&neg = 0,1)
// (+1, -1) -> 0  (pos&pos, neg&neg = 0,0)
// (0, X)   -> 0 
// Consensus with Learning:
// If A=0, B=X -> X
// If A=X, B=0 -> X
// If A=B -> A
// If A!=B -> 0 (Conflict)
TernaryWord TernaryWord::Consensus(const TernaryWord& other) const {
    // P_out = (P1 & ~N2) | (P2 & ~N1)
    // N_out = (N1 & ~P2) | (N2 & ~P1)
    
    uint64_t p = (pos & ~other.neg) | (other.pos & ~neg);
    uint64_t n = (neg & ~other.pos) | (other.neg & ~pos);
    
    return TernaryWord(p, n);
}

// Decay: Masking
// Apply mask to current state.
TernaryWord TernaryWord::Decay(const TernaryWord& mask) const {
    return TernaryWord(pos & mask.pos, neg & mask.neg);
}

// PopCount: Count non-zero trits
// Non-zero if either POS bit or NEG bit is set. (Valid trits are mutually exclusive).
#include <bitset>
int TernaryWord::PopCount() const {
    // Combine pos and neg to find all non-zero positions
    uint64_t non_zero = pos | neg;
    // Mask to just 27 trits to be safe (though upper bits should be 0)
    non_zero &= ((1ULL << NUM_TRITS) - 1);
    return std::bitset<64>(non_zero).count();
}

// Saturating Add: Add with Clamp on Overflow
TernaryWord TernaryWord::SaturatingAdd(const TernaryWord& other) const {
    uint64_t res_p = 0;
    uint64_t res_n = 0;
    int c_val = 0;
    
    for (int i = 0; i < NUM_TRITS; ++i) {
        int a = ((pos >> i) & 1) - ((neg >> i) & 1);
        int b = ((other.pos >> i) & 1) - ((other.neg >> i) & 1);
        int sum = a + b + c_val;
        
        int digit = 0;
        int new_c = 0;
        
        if (sum > 1) { digit = sum - 3; new_c = 1; }
        else if (sum < -1) { digit = sum + 3; new_c = -1; }
        else { digit = sum; new_c = 0; }
        
        c_val = new_c;
        if (digit == 1) res_p |= (1ULL << i);
        if (digit == -1) res_n |= (1ULL << i);
    }
    
    // Check Overflow
    if (c_val == 1) {
        // Overflow Max: Return all +1s
        return TernaryWord((1ULL << NUM_TRITS) - 1, 0);
    } else if (c_val == -1) {
        // Underflow Min: Return all -1s
        return TernaryWord(0, (1ULL << NUM_TRITS) - 1);
    }
    
    return TernaryWord(res_p, res_n);
}

// --- Encoding Standard ---

// Pack 27 trits into 54 bits (2 bits per trit)
// 00=0, 01=+1, 10=-1
uint64_t TernaryWord::ToPacked() const {
    uint64_t packed = 0;
    for (int i = 0; i < NUM_TRITS; ++i) {
        bool p = (pos >> i) & 1;
        bool n = (neg >> i) & 1;
        uint64_t code = 0;
        if (p) code = 1;      // 01
        else if (n) code = 2; // 10
        // else 00
        
        packed |= (code << (2 * i));
    }
    return packed;
}

// Unpack
TernaryWord TernaryWord::FromPacked(uint64_t val) {
    uint64_t p = 0;
    uint64_t n = 0;
    for (int i = 0; i < NUM_TRITS; ++i) {
        uint64_t code = (val >> (2 * i)) & 3; // Get 2 bits
        if (code == 1) p |= (1ULL << i);
        else if (code == 2) n |= (1ULL << i);
        // Ignore 3 (Reserved) and 0
    }
    return TernaryWord(p, n);
}
