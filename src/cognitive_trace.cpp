#include "cognitive_trace.h"
#include <fstream>
#include <iostream>
#include <iomanip>

namespace CognitiveTrace {

    void DumpPage(TernaryMemory& mem, int64_t page_id, const std::string& filename) {
        if (!mem.IsPageAllocated(page_id)) {
            std::cerr << "[Trace] Warning: Attempted to dump unallocated page " << page_id << std::endl;
            return;
        }

        std::ofstream out(filename);
        if (!out.is_open()) return;

        out << "Offset,Value,Trits\n";
        
        // We need to access the page data. 
        // TernaryMemory doesn't expose raw pointer easily, but we can Read loop.
        // Or friend class? For now, Read loop is fine for debug tools.
        int64_t base_addr = page_id * PAGE_SIZE; // 256 words
        
        for (int i = 0; i < PAGE_SIZE; ++i) {
            TernaryWord w = mem.Read(TernaryWord::FromInt64(base_addr + i));
            out << i << "," << w.ToInt64() << "," << w.ToString() << "\n";
        }
        
        out.close();
        std::cout << "[Trace] Dumped Page " << page_id << " to " << filename << std::endl;
    }

    void SnapshotBeliefs(TernaryMemory& mem, const std::string& filename_prefix, int64_t cycle) {
        // Iterate all possible cognitive pages? No, that's max 0x3000 to 0x7FFF / 256.
        // (32768 - 12288) / 256 = 80 pages.
        // We can just iterate them.
        
        // Better: TernaryMemory could expose list of allocated pages.
        // For now, scan the range.
        int64_t start_page = 0x3000 / PAGE_SIZE;
        int64_t end_page = 0x7FFF / PAGE_SIZE; // Exclusive? 0x8000 would be limit.
        
        for (int64_t p = start_page; p <= end_page; ++p) {
            if (mem.IsPageAllocated(p)) {
                 std::string fname = filename_prefix + "_cyc" + std::to_string(cycle) + "_p" + std::to_string(p) + ".csv";
                 DumpPage(mem, p, fname);
            }
        }
    }

    void LogStability(int64_t cycle, int64_t flux, int64_t energy, const std::string& filename) {
        std::ofstream out(filename, std::ios::app); // Append mode
        if (!out.is_open()) return;
        
        // Header check? Assumed external init or just append.
        out << cycle << "," << flux << "," << energy << "\n";
        out.close();
    }

}
