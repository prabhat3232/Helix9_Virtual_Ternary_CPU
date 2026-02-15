#include "memory.h"
#include <iostream>
#include <memory>

TernaryMemory::TernaryMemory() : current_context_id(0) {
    // System Memory: 3*243*9 (~12K words). Spec sets Reserved up to 0x2FFF.
    // 0x3000 = 12288 decimal.
    system_memory.resize(12288, TernaryWord::FromInt64(0));
}

// ... DecodeAddress ...

void TernaryMemory::AllocatePage(int64_t page_id, uint32_t owner, uint8_t perms) {
    if (!IsPageAllocated(page_id)) {
        auto p = std::make_shared<Page>();
        p->owner_id = owner;
        p->permissions = perms;
        cognitive_pages[page_id] = p;
    }
}

// Default Allocate (System Owner)
void TernaryMemory::AllocatePage(int64_t page_id) {
    AllocatePage(page_id, 0, PERM_OWNER_READ | PERM_OWNER_WRITE);
}

std::pair<int64_t, int64_t> TernaryMemory::DecodeAddress(const TernaryWord& address) {
    int64_t addr = address.ToInt64();
    if (addr < 0) addr = 0; // Clamp negative?
    
    // Page ID for Cognitive Memory
    int64_t page_id = addr / PAGE_SIZE;
    int64_t offset = addr % PAGE_SIZE;
    return {page_id, offset};
}

bool TernaryMemory::IsPageAllocated(int64_t page_id) const {
    return cognitive_pages.find(page_id) != cognitive_pages.end();
}


void TernaryMemory::OptimizePage(int64_t page_id) {
    if (!IsPageAllocated(page_id)) return;
    
    // Check if all zero
    bool all_zero = true;
    auto& p = cognitive_pages[page_id];
    for (int i=0; i<PAGE_SIZE; ++i) {
        if (p->words[i].ToInt64() != 0) {
            all_zero = false;
            break;
        }
    }
    
    if (all_zero) {
        cognitive_pages.erase(page_id);
    }
}

TernaryWord TernaryMemory::Read(const TernaryWord& address) {
    int64_t addr = address.ToInt64();
    
    // 1. System Memory (Fast Path)
    if (addr < 0x3000) {
        if (addr < 0 || addr >= (int64_t)system_memory.size()) return TernaryWord::FromInt64(0);
        return system_memory[addr];
    }
    
    // 2. Cognitive Memory (Sparse)
    auto pair = DecodeAddress(address);
    int64_t page_id = pair.first;
    int64_t offset = pair.second;
    
    if (IsPageAllocated(page_id)) {
        auto& p = cognitive_pages[page_id];
        // Permission Check
        if (current_context_id != 0 && current_context_id != p->owner_id) {
             // Access denied (Public access not implemented yet)
             std::cerr << "[MMU] Access Violation: Agent " << current_context_id 
                       << " cannot Read Page " << page_id << " (Owner " << p->owner_id << ")" << std::endl;
             // Trap? Return 0 for now.
             return TernaryWord::FromInt64(0);
        }
        return p->words[offset];
    }
    
    // Default-Read 0 for unallocated pages
    return TernaryWord::FromInt64(0);
}

void TernaryMemory::Write(const TernaryWord& address, const TernaryWord& value) {
    int64_t addr = address.ToInt64();
    
    // 1. System Memory
    if (addr < 0x3000) {
        if (addr >= 0 && addr < (int64_t)system_memory.size()) {
            system_memory[addr] = value;
        }
        return;
    }
    
    // 2. Cognitive Memory
    auto pair = DecodeAddress(address);
    int64_t page_id = pair.first;
    int64_t offset = pair.second;
    
    // Auto-allocate on Write
    if (!IsPageAllocated(page_id)) {
        // Optimization: Don't allocate if writing 0
        if (value.ToInt64() == 0) return;
        AllocatePage(page_id);
    }
    
    if (IsPageAllocated(page_id)) {
        auto& p = cognitive_pages[page_id];
        // Permission Check
        bool is_system = (current_context_id == 0);
        bool is_owner = (current_context_id == p->owner_id);
        
        if (!is_system && !is_owner) {
             std::cerr << "[MMU] Access Violation: Agent " << current_context_id 
                       << " cannot Write Page " << page_id << " (Owner " << p->owner_id << ")" << std::endl;
             return;
        }
        
        if (is_owner && !(p->permissions & PERM_OWNER_WRITE)) {
             std::cerr << "[MMU] Write Fault: Page " << page_id << " is Read-Only for Owner." << std::endl;
             return;
        }

        p->words[offset] = value;
    }
}

#include <fstream>
#include <string>

bool TernaryMemory::LoadFromFile(const std::string& filename, int64_t startAddr) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << "[MMU] Error: Could not open file " << filename << std::endl;
        return false;
    }
    
    int64_t currentAddr = startAddr;
    int64_t val;
    while (in >> val) {
        Write(TernaryWord::FromInt64(currentAddr++), TernaryWord::FromInt64(val));
    }
    
    in.close();
    std::cout << "[MMU] Loaded " << (currentAddr - startAddr) << " words from " << filename << std::endl;
    return true;
}

#include <sstream>

bool TernaryMemory::LoadExecutable(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << "[MMU] Error: Could not open " << filename << std::endl;
        return false;
    }

    std::string line;
    std::string sectionName = "";
    int64_t baseAddress = 0;
    int64_t expectedSize = 0;
    int64_t wordsLoaded = 0;

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string token;
        ss >> token;

        if (token == "HX") {
            int ver; ss >> ver; // Check version?
            continue;
        }
        
        if (token == "SECTION") {
            ss >> sectionName >> baseAddress >> expectedSize;
            // Read data lines
            if (std::getline(in, line)) {
                std::stringstream dss(line);
                int64_t val;
                int64_t currentAddr = baseAddress;
                while (dss >> val) {
                    Write(TernaryWord::FromInt64(currentAddr++), TernaryWord::FromInt64(val));
                    wordsLoaded++;
                }
            }
        }
    }
    
    in.close();
    std::cout << "[MMU] Loaded Executable " << filename << " (" << wordsLoaded << " words)" << std::endl;
    return true;
}
