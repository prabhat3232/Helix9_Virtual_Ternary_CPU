#pragma once
#include "soft_float.h"
#include <vector>
#include <iostream>

// Sparse Vector: Parallel Vectors (SoA) implementation
// indices: stores the column index
// values: stores the TernaryFloat value
class SparseVector {
public:
    std::vector<int> indices;
    std::vector<TernaryFloat> values;
    int logical_size; // The theoretical size of the vector (input dimension)

    SparseVector(int s = 0) : logical_size(s) {}

    void Set(int idx, TernaryFloat val) {
        // If value is 0, we can prune it or not add it.
        // For learning, we might want to keep it if it was previously non-zero.
        // But for initialization, if it's 0, we often skip.
        if (val.mantissa == 0) return;

        // Check availability (Linear scan is acceptable for construction/low sparsity)
        for(size_t k=0; k<indices.size(); ++k) {
            if (indices[k] == idx) {
                values[k] = val;
                return;
            }
        }
        
        // Add new
        indices.push_back(idx);
        values.push_back(val);
    }

    TernaryFloat Get(int idx) const {
        for(size_t k=0; k<indices.size(); ++k) {
            if (indices[k] == idx) return values[k];
        }
        return TernaryFloat(0, 0);
    }
};

class SparseMatrix {
public:
    int rows;
    int cols;
    std::vector<SparseVector> row_vectors; // [Row] -> SparseVector

    SparseMatrix(int r, int c) : rows(r), cols(c) {
        row_vectors.resize(r, SparseVector(c));
    }
    
    // Sparse Dot Product: Row * Vector
    TernaryFloat DotRow(int r, const std::vector<TernaryFloat>& input) const {
        TernaryFloat sum(0, 0);
        const auto& vec = row_vectors[r];
        
        // Linear iteration over non-zero elements
        for(size_t k=0; k<vec.indices.size(); ++k) {
            int idx = vec.indices[k];
            if (idx < input.size()) {
                 // Sum += Weight * Input
                 sum = sum.Add(vec.values[k].Multiply(input[idx]));
            }
        }
        return sum;
    }
};
