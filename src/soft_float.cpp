#include "soft_float.h"
#include <iostream>
#include <iomanip>
#include <cmath>

// =========================================================
// OPTIMIZATION: Pre-computed Powers of 3
// =========================================================

// Integer powers of 3 for bit-shifting mantissas (up to 3^39 to fit in int64)
// 3^0 to 3^39
static const int64_t POW_3_INT[] = {
    1L, 3L, 9L, 27L, 81L, 243L, 729L, 2187L, 6561L, 19683L,
    59049L, 177147L, 531441L, 1594323L, 4782969L, 14348907L, 43046721L,
    129140163L, 387420489L, 1162261467L, 3486784401L, 10460353203L,
    31381059609L, 94143178827L, 282429536481L, 847288609443L,
    2541865828329L, 7625597484987L, 22876792454961L, 68630377364883L,
    205891132094649L, 617673396283947L, 1853020188851841L, 5559060566555523L,
    16677181699666569L, 50031545098999707L, 150094635296999121L,
    450283905890997363L, 1350851717672992089L, 4052555153018976267L
};

// Constant for FromDouble (3^12)
static const double SCALE_FACTOR_12 = 531441.0;

// =========================================================
// IMPLEMENTATION
// =========================================================

double TernaryFloat::ToDouble() const {
    // Optimization: Use pre-computed power if within range
    double scale;
    int64_t abs_exp = std::abs(exponent);

    if (abs_exp < 40) {
        scale = (double)POW_3_INT[abs_exp];
    } else {
        // Fallback for extreme values
        scale = std::pow(3.0, (double)abs_exp);
    }

    if (exponent < 0) {
        return (double)mantissa / scale;
    } else {
        return (double)mantissa * scale;
    }
}

TernaryFloat TernaryFloat::FromDouble(double value) {
    if (value == 0) return TernaryFloat(0, 0);

    // OPTIMIZATION: Hardcoded precision shift of 12
    // Removed std::pow(3.0, 12) - now uses constant
    int64_t m = std::round(value * SCALE_FACTOR_12);
    int64_t e = -12;
    
    // Normalize: remove trailing zeros (divisible by 3)
    while (m != 0 && m % 3 == 0) {
        m /= 3;
        e++;
    }
    
    return TernaryFloat(m, e);
}

TernaryFloat TernaryFloat::Add(const TernaryFloat& other) const {
    int64_t exp_a = exponent;
    int64_t exp_b = other.exponent;
    int64_t man_a = mantissa;
    int64_t man_b = other.mantissa;
    
    // Fast path: same exponent
    if (exp_a == exp_b) {
        return TernaryFloat(man_a + man_b, exp_a);
    }

    // OPTIMIZATION: Integer Lookup instead of std::pow
    if (exp_a > exp_b) {
        int64_t diff = exp_a - exp_b;
        if (diff >= 39) {
            return *this; // Smaller number vanishes
        }
        man_a *= POW_3_INT[diff];
        exp_a = exp_b;
    } else {
        int64_t diff = exp_b - exp_a;
        if (diff >= 39) {
            return other;
        }
        man_b *= POW_3_INT[diff];
        exp_b = exp_a;
    }
    
    return TernaryFloat(man_a + man_b, exp_a);
}

TernaryFloat TernaryFloat::Multiply(const TernaryFloat& other) const {
    // Already optimal: (M1*M2) * 3^(E1+E2)
    return TernaryFloat(mantissa * other.mantissa, exponent + other.exponent);
}

TernaryFloat TernaryFloat::ReLU() const {
    // OPTIMIZATION: Check mantissa directly instead of ToDouble()
    if (mantissa > 0) {
        return *this;
    }
    
    // Leaky ReLU for negative values
    return TernaryFloat::FromDouble(ToDouble() * 0.01);
}

std::string TernaryFloat::ToString() const {
    return std::to_string(mantissa) + "e" + std::to_string(exponent) + " (" + std::to_string(ToDouble()) + ")";
}
