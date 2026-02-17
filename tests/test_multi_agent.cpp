
#include <iostream>
#include <vector>
#include <memory>
#include <cstdint>
#include <cmath>
#include "../src/cpu.h"
#include "../src/cognitive/scheduler.h"
#include "../src/assembler/Parser.h" // Defines ObjectFile
#include "../src/linker/Linker.h"
#include <fstream>
#include <sstream>

using namespace Helix;
using namespace Helix::Cognitive;

// Helper to compile and load ASM
bool SetupAgents(Cpu& cpu, Scheduler& scheduler, const std::string& asm_path, int num_agents) {
    // 1. Assemble
    Helix::Assembler::Parser parser(asm_path); // Wait, Parser takes source string, not path?
    // Parser constructor reads file? No, usually source content.
    // Let's read file content manually.
    std::ifstream t(asm_path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    Helix::Assembler::Parser p(buffer.str());
    Helix::Assembler::ObjectFile obj = p.Compile();
    
    // 2. Link (Minimal link to resolve symbols if any - simplistic, just use obj data)
    // We can assume single section .text at 0x0000
    // Load into CPU Memory
    uint64_t load_addr = 0;
    if (!obj.sections.empty()) {
        for(const auto& word : obj.sections[0].data) {
            cpu.mem.Write(TernaryWord::FromInt64(load_addr++), word);
        }
    }
    
    // 3. Create Agents
    for(int i=0; i<num_agents; ++i) {
        auto agent = std::make_shared<Agent>(i + 1);
        agent->pc = 0; // Start of code
        agent->sp = 0x7F00 - (i * 0x100); // Separate stacks
        agent->status = 0;
        agent->state = AgentState::ACTIVE; // Start Running
        scheduler.RegisterAgent(agent);
    }
    return true;
}

int main() {
    std::cout << "[Test] Multi-Agent Concurrency" << std::endl;
    
    TernaryMemory mem;
    Cpu cpu(mem);
    Scheduler scheduler(cpu);
    
    // Setup 2 agents running 'agent_task.hasm'
    // We assume 'agent_task.hasm' is compiled and loaded.
    // Since we don't have easy file path access in compiled test relative run,
    // let's embed the source code string?
    std::string source = R"(
        ldi.w r1 0
        ldi.w r2 1
    loop:
        add.w r1 r1 r2
        jmp loop
    )";
    
    Helix::Assembler::Parser p(source);
    Helix::Assembler::ObjectFile obj = p.Compile();
    
    // Load to Memory
    uint64_t addr = 0;
    if (!obj.sections.empty()) {
        for(const auto& word : obj.sections[0].data) {
            cpu.mem.Write(TernaryWord::FromInt64(addr++), word);
        }
    }
    
    // Create Agents
    auto a1 = std::make_shared<Agent>(1);
    a1->pc = 0;
    a1->regs.resize(16, 0); // Ensure size
    a1->state = AgentState::ACTIVE;
    scheduler.RegisterAgent(a1);
    
    auto a2 = std::make_shared<Agent>(2);
    a2->pc = 0;
    a2->regs.resize(16, 0);
    a2->state = AgentState::ACTIVE;
    scheduler.RegisterAgent(a2);
    
    scheduler.SetMaxCyclesPerAgent(10); // Run 10 cycles per tick per agent
    // cpu.ToggleTrace(true);
    
    // Run for 10 ticks
    for(int i=0; i<10; ++i) {
        scheduler.Tick();
        
        // Check states
        // Agents should have advanced R1.
        // Tick 0: A1 runs 10 cycles. R1 approx 3? 
        // Tick 1: A2 runs 10 cycles. R1 approx 3? 
        // Tick 2: A1 resumes...
        
        std::cout << "Tick " << i << ": A1.R1=" << a1->regs[1] << " A2.R1=" << a2->regs[1] << std::endl;
    }
    
    // Validations
    if (a1->regs[1] > 0 && a2->regs[1] > 0) {
        std::cout << "[Success] Both agents made progress." << std::endl;
    } else {
        std::cout << "[Failure] Agents did not progress." << std::endl;
        return 1;
    }
    
    // Verify isolation
    // Ideally values should be similar if code is deterministic and fair scheduling.
    int64_t diff = std::abs((long long)a1->regs[1] - (long long)a2->regs[1]);
    if (diff > 5) { // Allow small jitter
        std::cout << "[Warning] Skew detected: " << diff << std::endl;
    }
    
    return 0;
}
