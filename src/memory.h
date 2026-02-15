#pragma once
#include "trit_word.h"
#include <unordered_map>
#include <vector>
#include <memory>

// Page Size: 3^9 = 19683 words
// Page Size: 256 words (Architecture Spec)
const int64_t PAGE_SIZE = 256; 
// Old PAGE_SIZE was 19683. We are changing to 256 for Cognitive Spec.
// But we need to keep legacy support for flat memory?
// Legacy code assumed 32K words flat.
// New Model:
// 0x0000 - 0x2FFF: System (Flat Vector)
// 0x3000 - 0x7FFF: Cognitive (Sparse Pages of 256 words)

// Memory Permissions
const uint8_t PERM_OWNER_READ = 0x01;
const uint8_t PERM_OWNER_WRITE = 0x02;

struct Page {
    std::vector<TernaryWord> words;
    uint32_t owner_id;
    uint8_t permissions; 
    
    Page() : owner_id(0), permissions(PERM_OWNER_READ | PERM_OWNER_WRITE) { 
        words.resize(PAGE_SIZE, TernaryWord::FromInt64(0)); 
    }
};

class TernaryMemory {
private:
    std::vector<TernaryWord> system_memory; // 0x0000 - 0x2FFF
    std::unordered_map<int64_t, std::shared_ptr<Page>> cognitive_pages; // PageID -> Page
    uint32_t current_context_id; // 0 = System (Root)

public:
    TernaryMemory();
    
    // Context Management
    void SetContext(uint32_t context_id) { current_context_id = context_id; }
    uint32_t GetContext() const { return current_context_id; }
    
    // Read/Write
    TernaryWord Read(const TernaryWord& address);
    void Write(const TernaryWord& address, const TernaryWord& value);
    
    // Sparse Helpers
    bool IsPageAllocated(int64_t page_id) const;
    void AllocatePage(int64_t page_id);
    void AllocatePage(int64_t page_id, uint32_t owner, uint8_t perms); // Overload
    void OptimizePage(int64_t page_id); // Check if empty -> Deallocate

    // Legacy Loading
    bool LoadExecutable(const std::string& filename);
    bool LoadFromFile(const std::string& filename, int64_t startAddr);
    
    static std::pair<int64_t, int64_t> DecodeAddress(const TernaryWord& address);
};
