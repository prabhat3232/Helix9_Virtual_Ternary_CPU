#pragma once
#include <cstdint>
#include <string>

// Represents a 27-trit balanced ternary word using 2-bit biased encoding.
class TernaryWord {
public:
    uint64_t pos; // Positive bits
    uint64_t neg; // Negative bits

    TernaryWord() : pos(0), neg(0) {}
    TernaryWord(uint64_t p, uint64_t n) : pos(p), neg(n) {}
    
    // Create from integer (for testing)
    static TernaryWord FromInt64(int64_t value);
    int64_t ToInt64() const;

    // Logic Operations
    TernaryWord Min(const TernaryWord& other) const;
    TernaryWord Max(const TernaryWord& other) const;
    TernaryWord Negate() const;
    TernaryWord XOR(const TernaryWord& other) const; // Sum Mod 3
    
    TernaryWord ShiftLeft() const;  // LSL
    TernaryWord ShiftRight() const; // LSR

    // Arithmetic Operations
    TernaryWord Add(const TernaryWord& other) const;
    TernaryWord SaturatingAdd(const TernaryWord& other) const; // Cognitive Mode

    // Cognitive Operations (Phase 6)
    TernaryWord Consensus(const TernaryWord& other) const; // A=B->A, else 0
    TernaryWord Decay(const TernaryWord& mask) const;      // Intersect with mask
    int PopCount() const;                                  // Count non-zero trits

    // Encoding (Phase 6)
    uint64_t ToPacked() const;                             // 2-bit packing
    static TernaryWord FromPacked(uint64_t val);

    // Bit-Slicing (for Instruction Decoding)
    int8_t GetTrit(int index) const;
    void SetTrit(int index, int8_t val);
    
    // Slice: Extract value from [start, start+len-1]
    int64_t Slice(int start, int len) const;
    
    // SetSlice: Set value into [start, start+len-1]
    void SetSlice(int start, int len, int64_t val);

    // Helpers
    std::string ToString() const;
};

