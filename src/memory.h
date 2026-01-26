#pragma once
#include "trit_word.h"
#include <unordered_map>
#include <vector>

// Page Size: 3^9 = 19683 words
const int64_t PAGE_SIZE = 19683;

class TernaryMemory {
private:
    // Map Page ID (high trits) to Page Data
    std::unordered_map<int64_t, std::vector<TernaryWord>> pages;

public:
    TernaryMemory();
    
    // Read/Write word at address
    TernaryWord Read(const TernaryWord& address);
    void Write(const TernaryWord& address, const TernaryWord& value);
    
    // Allocate a page if it doesn't exist
    void MapPage(int64_t page_id);
    
    // Helper: Address -> (PageID, Offset)
    static std::pair<int64_t, int64_t> DecodeAddress(const TernaryWord& address);
};
