#pragma once
#include "soft_float.h"
#include <vector>
#include <iostream>

class VectorALU {
public:
    static TernaryFloat DotProduct(const std::vector<TernaryFloat>& A, const std::vector<TernaryFloat>& B) {
        if (A.size() != B.size()) {
            std::cerr << "[VectorALU] Error: Vector dimension mismatch." << std::endl;
            return TernaryFloat(0, 0);
        }
        
        TernaryFloat sum(0, 0);
        
        // This loop simulates SIMD parallel multiplication (accumulating)
        for (size_t i = 0; i < A.size(); ++i) {
            TernaryFloat product = A[i].Multiply(B[i]);
            sum = sum.Add(product);
        }
        
        return sum;
    }
};
