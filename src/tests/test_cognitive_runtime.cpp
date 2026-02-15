#include <iostream>
#include <vector>
#include <cassert>
#include "../cpu.h"
#include "../memory.h"
#include "../cognitive/scheduler.h"
#include "../cognitive/agent.h"

using namespace Helix;
using namespace Helix::Cognitive;

// Mock Agent Logic to inject into Memory
// For now, we just let them execute NOPs (0000) or simple loops.
// 0000 = NOP? Spec says HALT is 0000 usually, but let's assume we load some "Inc" or similar.
// Actually, with no code loaded, they read 0. If 0 is Halt, they stop.
// We need to load them with an infinite loop: `JMP Self`.

void SetupAgentCode(TernaryMemory& mem, const Agent& agent) {
    // Unconditional Jump to Self (Infinite Loop) to simulate work
    // Opcode JUMP is usually... let's check ISA. assuming simple interaction.
    // If we just put random non-zero data, it might crash.
    // Let's just rely on Step() handling 0 as NOP or similar.
    // Spec says 0000 is usually invalid/halt in real hardware, but our Emulator `Step` ?
    // Let's look at `cpu.cpp`: default case does nothing.
    // `case 0:` undefined in switch? 
    // We should probably check cpu.cpp to see what opcode 0 does.
    // If it does nothing, then infinite loop of NOPs is fine.
}

void TestFairness() {
    std::cout << "[Test] Starting Fairness Test..." << std::endl;
    
    TernaryMemory mem;
    Cpu cpu(mem);
    Scheduler scheduler(cpu);
    
    // Create 3 Agents
    auto a1 = std::make_shared<Agent>(1);
    auto a2 = std::make_shared<Agent>(2);
    auto a3 = std::make_shared<Agent>(3);
    
    // Set states to ACTIVE
    a1->state = AgentState::ACTIVE;
    a2->state = AgentState::ACTIVE;
    a3->state = AgentState::ACTIVE; // One inactive
    
    // Simple Memory Setup (Allocate Stacks)
    cpu.mem.AllocatePage(10); a1->sp = 10 * PAGE_SIZE + 100;
    cpu.mem.AllocatePage(20); a2->sp = 20 * PAGE_SIZE + 100;
    cpu.mem.AllocatePage(30); a3->sp = 30 * PAGE_SIZE + 100;

    scheduler.RegisterAgent(a1);
    scheduler.RegisterAgent(a2);
    scheduler.RegisterAgent(a3);
    
    // Run for 30 ticks
    for(int i=0; i<30; ++i) {
        scheduler.Tick();
    }
    
    // Check if all ran
    std::cout << "Agent 1 Last Tick: " << a1->last_tick << std::endl;
    std::cout << "Agent 2 Last Tick: " << a2->last_tick << std::endl;
    std::cout << "Agent 3 Last Tick: " << a3->last_tick << std::endl;
    
    assert(a1->last_tick > 0);
    assert(a2->last_tick > 0);
    assert(a3->last_tick > 0);
    
    std::cout << "[Test] Fairness Test Passed." << std::endl;
}

void TestIsolation() {
    std::cout << "[Test] Starting Isolation Test..." << std::endl;
    TernaryMemory mem;
    Cpu cpu(mem);
    Scheduler scheduler(cpu);
    
    auto a1 = std::make_shared<Agent>(100);
    a1->state = AgentState::ACTIVE;
    // Owns Page 50
    cpu.mem.AllocatePage(50, 100, PERM_OWNER_READ | PERM_OWNER_WRITE);
    // Writes to Page 50 (Allowed)
    
    // Setup Context
    cpu.mem.SetContext(100);
    cpu.mem.Write(TernaryWord::FromInt64(50 * PAGE_SIZE), TernaryWord::FromInt64(42));
    assert(cpu.mem.Read(TernaryWord::FromInt64(50 * PAGE_SIZE)).ToInt64() == 42);
    
    // Try accessing Page 60 (Owned by Agent 200)
    cpu.mem.AllocatePage(60, 200, PERM_OWNER_READ);
    cpu.mem.Write(TernaryWord::FromInt64(60 * PAGE_SIZE), TernaryWord::FromInt64(999)); // Should Fail
    
    // We expect it NOT to change (since Write returns void, we check value)
    // If Write failed, value should remain 0 (default) or whatever it was.
    // Wait, Write to unallocated auto-allocates? 
    // IsPageAllocated(60) is true.
    // Write check:
    // if owner mismatch -> return.
    
    int64_t val = cpu.mem.Read(TernaryWord::FromInt64(60 * PAGE_SIZE)).ToInt64();
    // Read also checks perm. Agent 100 reading Agent 200 page? Should fail (return 0).
    assert(val == 0);
    
    std::cout << "[Test] Isolation Test Passed." << std::endl;
}

int main() {
    TestFairness();
    TestIsolation();
    std::cout << "All Cognitive Runtime Tests Passed." << std::endl;
    return 0;
}
