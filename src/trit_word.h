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

    // Arithmetic Operations
    TernaryWord Add(const TernaryWord& other) const;

    // Debug
    std::string ToString() const;
};
