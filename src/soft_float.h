#pragma once
#include "trit_word.h"
#include <cmath>

// Ternary27 Standard
// Mantissa: 20 Trits
// Exponent: 7 Trits
class TernaryFloat {
public:
    int64_t mantissa; // Stored as integer for simplicity in PoC
    int64_t exponent; // Stored as integer

    TernaryFloat() : mantissa(0), exponent(0) {}
    TernaryFloat(int64_t m, int64_t e) : mantissa(m), exponent(e) {}

    // Construct from double (approximation)
    static TernaryFloat FromDouble(double value);
    double ToDouble() const;

    // Arithmetic
    TernaryFloat Add(const TernaryFloat& other) const;
    TernaryFloat Multiply(const TernaryFloat& other) const;
    
    // Activation
    TernaryFloat ReLU() const;
    
    // Debug
    std::string ToString() const;
};
