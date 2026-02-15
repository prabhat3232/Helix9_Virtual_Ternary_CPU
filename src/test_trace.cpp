#include "memory.h"
#include "trit_word.h"
#include "cognitive_trace.h"
#include <iostream>
#include <fstream>
#include <cstdio>

// Assert Helper
void Assert(bool cond, const std::string& msg) {
    if (!cond) {
        std::cerr << "FAILED: " << msg << std::endl;
        exit(1);
    } else {
        std::cout << "PASSED: " << msg << std::endl;
    }
}

int main() {
    std::cout << "--- Testing Cognitive Trace ---" << std::endl;
    
    TernaryMemory mem;
    int64_t page_id = 0x3000 / PAGE_SIZE; // First cognitive page (Page 48)
    
    // 1. Allocate and Populate Page
    mem.Write(TernaryWord::FromInt64(0x3000 + 0), TernaryWord::FromInt64(42));
    mem.Write(TernaryWord::FromInt64(0x3000 + 1), TernaryWord::FromInt64(-121));
    mem.Write(TernaryWord::FromInt64(0x3000 + 255), TernaryWord::FromInt64(1)); // End of page
    
    // 2. Dump Page
    std::string dump_file = "trace_dump_test.csv";
    CognitiveTrace::DumpPage(mem, page_id, dump_file);
    
    // Verify File Exists and Content
    std::ifstream in(dump_file);
    Assert(in.is_open(), "Dump file created");
    
    std::string line;
    std::getline(in, line); 
    Assert(line == "Offset,Value,Trits", "Header correct");
    
    std::getline(in, line);
    // 0,42,000...11T0
    // Checking rudimentary content match
    Assert(line.find("0,42,") == 0, "Row 0 correct");
    
    in.close();
    
    // 3. Test Snapshot
    CognitiveTrace::SnapshotBeliefs(mem, "snap", 100);
    // Should create snap_cyc100_p48.csv
    std::string snap_file = "snap_cyc100_p" + std::to_string(page_id) + ".csv";
    std::ifstream snap_in(snap_file);
    Assert(snap_in.is_open(), "Snapshot file created");
    snap_in.close();
    
    // 4. Test Stability Log
    std::string log_file = "stability_test.csv";
    CognitiveTrace::LogStability(1, 10, 50, log_file);
    CognitiveTrace::LogStability(2, 8, 45, log_file);
    
    std::ifstream log_in(log_file);
    Assert(log_in.is_open(), "Stability log created");
    std::getline(log_in, line);
    Assert(line == "1,10,50", "Log Row 1 correct");
    std::getline(log_in, line);
    Assert(line == "2,8,45", "Log Row 2 correct");
    log_in.close();

    // Cleanup
    remove(dump_file.c_str());
    remove(snap_file.c_str());
    remove(log_file.c_str());
    
    std::cout << "--- Cognitive Trace Verified ---" << std::endl;
    return 0;
}
