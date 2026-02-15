#include "agent.h"
#include "scheduler.h"
#include <iostream>
#include <algorithm>

namespace Helix {
namespace Cognitive {

Scheduler::Scheduler(Cpu& cpu_ref) 
    : cpu(cpu_ref), cognitive_tick_count(0), 
      cpu_cycles_per_tick(1000), max_agent_cycles(100) {}

void Scheduler::RegisterAgent(std::shared_ptr<Agent> agent) {
    agents.push_back(agent);
    // std::cout << "[Scheduler] Registered Agent " << agent->id << std::endl;
}

void Scheduler::TerminateAgent(uint32_t agent_id) {
    auto it = std::remove_if(agents.begin(), agents.end(), 
        [agent_id](const std::shared_ptr<Agent>& a) { return a->id == agent_id; });
    
    if (it != agents.end()) {
        agents.erase(it, agents.end());
        std::cout << "[Scheduler] Terminated Agent " << agent_id << std::endl;
    }
}

void Scheduler::SetCognitiveTickRate(uint64_t cycles_per_tick) {
    cpu_cycles_per_tick = cycles_per_tick;
}

void Scheduler::SetMaxCyclesPerAgent(uint64_t cycles) {
    max_agent_cycles = cycles;
}

uint64_t Scheduler::Tick() {
    // 1. Core Scheduler Loop
    ScheduleRoundRobin();

    // 2. Advance Cognitive Time
    cognitive_tick_count++;

    // 3. Return equivalent CPU cost (for simulation accounting)
    return cpu_cycles_per_tick;
}

void Scheduler::ScheduleRoundRobin() {
    // Basic Round-Robin with Priority Weighting (TODO: Add weighted logic later)
    for (auto& agent : agents) {
        if (agent->state == AgentState::ACTIVE || agent->state == AgentState::LEARNING) {
            ExecuteAgent(agent);
        }
    }
}

void Scheduler::ExecuteAgent(std::shared_ptr<Agent> agent) {
    // 1. SENSE: (Placeholder for now, will copy Input Pages)
    (void)agent; // Prevent unused warning for now

    // 2. CONTEXT SWITCH
    // Save current "System" context (optional, or we assume System matches Scheduler)
    // For now, Helix is singular. We just load Agent PC/SP.
    
    TernaryWord system_pc = cpu.pc;
    TernaryWord system_sp = cpu.regs[13]; // SP

    // Load Agent
    cpu.pc = TernaryWord::FromInt64(agent->pc);
    cpu.regs[13] = TernaryWord::FromInt64(agent->sp);
    
    // 3. RUN QUANTUM
    uint64_t cycles_used = cpu.Step(max_agent_cycles);
    
    // 4. UPDATE AGENT STATE
    agent->pc = cpu.pc.ToInt64();
    agent->sp = cpu.regs[13].ToInt64();
    agent->last_tick = cognitive_tick_count;

    // Restore System?
    // In a real OS, yes. Here, the Scheduler IS the system code running on the same CPU.
    // So we must rely on C++ stack preservation or manual restore.
    // Actually, `Step` manages the virtual CPU loop. The Scheduler is C++ code driving it.
    // So we just restore the Virtual CPU state to "Idle/System" if needed.
    // For now, let's leave CPU state as "last agent left it" unless we have a specific System Idle loop.
    // But to be safe, let's restore System registers if we had a "System Mode".
    // ...
    
    cpu.pc = system_pc;
    cpu.regs[13] = system_sp;
    
    // 5. STABILIZE: Compute Metrics (Flux, Energy)
    stability_monitor.CaptureState(*agent, cpu.mem);
    StabilityMetrics metrics = stability_monitor.Analyze(agent->id);
    
    agent->stability_index = (int64_t)(metrics.flux * 100.0); // 0-100 scale
    // TODO: Update Agent Health based on Energy/Flux?
    
    // 6. REWARD: Apply reinforcement if LEARNING
    if (agent->state == Helix::Cognitive::AgentState::LEARNING) {
        // Simple Intrinsic Reward:
        // Reward = (Stability + Coherence) - Energy
        int64_t raw_reward = 0;
        
        // Example: We want Low Flux (Stability)
        if (metrics.converged) raw_reward += 10;
        else raw_reward -= 5;
        
        reward_engine.ApplyReward(*agent, raw_reward);
    }
}

} // namespace Cognitive
} // namespace Helix
