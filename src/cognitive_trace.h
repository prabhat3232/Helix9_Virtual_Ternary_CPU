#pragma once
#include "memory.h"
#include <string>
#include <vector>

// Debug/Trace Utilities for Cognitive Research
namespace CognitiveTrace {

    // Dump a Memory Page to a CSV/Log file
    // Format: Address, Value(Int), Value(TritString)
    void DumpPage(TernaryMemory& mem, int64_t page_id, const std::string& filename);

    // Snapshot Entrie Belief State (All allocated Cognitive Pages)
    void SnapshotBeliefs(TernaryMemory& mem, const std::string& filename_prefix, int64_t cycle);

    // Log Stability Metric (e.g. Total Energy / Flux)
    void LogStability(int64_t cycle, int64_t flux, int64_t energy, const std::string& filename);

}
