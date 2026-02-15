#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include "../src/cpu.h"
#include "../src/memory.h"

// Simple Harness to load .ht files and time execution
// using namespace Helix; // cpu.h is global

struct BenchResult {
    std::string name;
    uint64_t cycles;
    double time_ms;
    double mips;
};

BenchResult RunBenchmark(const std::string& name, const std::string& filename) {
    TernaryMemory mem;
    Cpu cpu(mem);
    
    // Load .ht Executable
    if (!mem.LoadExecutable(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
        return {name, 0, 0.0, 0.0};
    }

    std::cout << "Running " << name << "..." << std::endl;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    // Run until HLT
    while (true) {
        cpu.Step(100); // Batch steps
        if (cpu.halted) break;
        if (cpu.metrics.total_cycles > 10000000) { // Safety break
            std::cerr << "Timeout!" << std::endl;
            break;
        }
    }
    
    uint64_t total_cycles = cpu.metrics.total_cycles;
    
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end_time - start_time;
    
    // Real MIPS = (Instructions) / Seconds. 
    // MIPS usually means native instructions. 
    // Emulation MIPS = (Active Cycles) / Seconds?
    // Let's stick to Cycles/Sec for now or just report Cycles.
    double real_mips = (total_cycles / 1000000.0) / (duration.count() / 1000.0);

    return {name, total_cycles, duration.count(), real_mips};
}

int main(int argc, char** argv) {
    std::cout << "Helix9 Benchmark Suite v1.0" << std::endl;
    std::cout << "---------------------------" << std::endl;

    std::vector<BenchResult> results;
    
    // Expect compiled .ht files in current dir or specific path
    // We assume they are pre-compiled or we verify assembly first.
    
    results.push_back(RunBenchmark("Base Arithmetic", "benchmarks/bench_base.ht"));
    results.push_back(RunBenchmark("Cognitive Ops", "benchmarks/bench_cog.ht"));
    results.push_back(RunBenchmark("Agent Cycle", "benchmarks/bench_agent.ht"));
    results.push_back(RunBenchmark("Vector Soft (256)", "benchmarks/bench_vec_soft.ht"));
    results.push_back(RunBenchmark("Vector Hard (256)", "benchmarks/bench_vec_hard.ht"));
    
    std::cout << "\nResults:" << std::endl;
    std::cout << std::left << std::setw(20) << "Benchmark" 
              << std::setw(15) << "Cycles" 
              << std::setw(15) << "Time (ms)" 
              << std::setw(15) << "MIPS (Emu)" << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;
    
    for(const auto& r : results) {
        std::cout << std::left << std::setw(20) << r.name 
                  << std::setw(15) << r.cycles 
                  << std::setw(15) << std::fixed << std::setprecision(3) << r.time_ms 
                  << std::setw(15) << std::setprecision(2) << r.mips << std::endl;
    }

    return 0;
}
