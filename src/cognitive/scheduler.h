#pragma once
#include <vector>
#include <memory>
#include <vector>
#include <memory>
// #include "agent.h" // Removed to prevent circular dependency
#include "stability_monitor.h"
#include "reward_engine.h"
#include "../memory.h"
#include "../cpu.h"

namespace Helix {
namespace Cognitive {

struct Agent; // Forward Declaration

class Scheduler {
public:
    Scheduler(Cpu& cpu_ref);

    // Lifecycle
    void RegisterAgent(std::shared_ptr<Agent> agent);
    void TerminateAgent(uint32_t agent_id);
    
    // The Core Loop (Cognitive Tick)
    // Returns number of CPU cycles consumed
    uint64_t Tick();

    // Configuration
    void SetCognitiveTickRate(uint64_t cycles_per_tick);
    void SetMaxCyclesPerAgent(uint64_t cycles);

private:
    Cpu& cpu;
    std::vector<std::shared_ptr<Agent>> agents;
    uint64_t cognitive_tick_count;
    
    StabilityMonitor stability_monitor;
    RewardEngine reward_engine;
    
    // Config
    uint64_t cpu_cycles_per_tick; // Ratio
    uint64_t max_agent_cycles;    // Fairness Limit

    // Helpers
    void ScheduleRoundRobin();
    void ExecuteAgent(std::shared_ptr<Agent> agent);
    void UpdateState(std::shared_ptr<Agent> agent);
};

} // namespace Cognitive
} // namespace Helix
