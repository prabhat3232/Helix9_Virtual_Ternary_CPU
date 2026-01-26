#include "memory.h"
#include <iostream>

TernaryMemory::TernaryMemory() {}

std::pair<int64_t, int64_t> TernaryMemory::DecodeAddress(const TernaryWord& address) {
    int64_t linear_addr = address.ToInt64();
    
    // Handle balanced ternary indexing if needed. 
    // For simplicity in PoC, we map TernaryWord integer value directly to a linear space.
    // If Int64 is negative, we just map it.
    
    int64_t page_id = linear_addr / PAGE_SIZE;
    int64_t offset = linear_addr % PAGE_SIZE;
    
    if (offset < 0) {
        offset += PAGE_SIZE;
        page_id -= 1;
    }
    
    return {page_id, offset};
}

void TernaryMemory::MapPage(int64_t page_id) {
    if (pages.find(page_id) == pages.end()) {
        // Create blank page filled with 0s
        pages[page_id] = std::vector<TernaryWord>(PAGE_SIZE, TernaryWord::FromInt64(0));
        // std::cout << "[MMU] Allocated Page " << page_id << std::endl;
    }
}

TernaryWord TernaryMemory::Read(const TernaryWord& address) {
    auto pair = DecodeAddress(address);
    int64_t page = pair.first;
    int64_t offset = pair.second;
    
    if (pages.find(page) == pages.end()) {
        std::cerr << "[MMU] Segmentation Fault: Read at unmapped page " << page << std::endl;
        return TernaryWord::FromInt64(0);
    }
    
    return pages[page][offset];
}

void TernaryMemory::Write(const TernaryWord& address, const TernaryWord& value) {
    auto pair = DecodeAddress(address);
    int64_t page = pair.first;
    int64_t offset = pair.second;
    
    if (pages.find(page) == pages.end()) {
        MapPage(page); // Auto-map on write for PoC convenience
    }
    
    pages[page][offset] = value;
}
