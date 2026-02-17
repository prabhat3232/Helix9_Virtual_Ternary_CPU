#pragma once
#include <cstdint>
#include <vector>
#include "../trit_word.h"

namespace Helix {
namespace Cognitive {

// Agent Lifecycle States
enum class AgentState : uint8_t {
    INIT = 0,       // Allocating resources
    ACTIVE = 1,     // Normal Execution (No Learning)
    LEARNING = 2,   // Plasticity Enabled (Reward Active)
    CONVERGED = 3,  // Stable (Low Power)
    IDLE = 4,       // Suspended/Paused
    TERMINATED = 5  // Dead/Cleanup
};

struct Agent {
    uint32_t id;
    AgentState state;
    uint8_t priority;       // 0-255 (Higher is better)
    uint64_t last_tick;     // Cognitive Tick of last execution

    // Memory Ownership (Page IDs)
    // Enforced by Runtime Kernel
    uint16_t belief_page_start;  // EXCLUSIVE R/W
    uint16_t belief_page_count;
    
    uint16_t input_page_start;   // READ-ONLY (Env Writes)
    uint16_t output_page_start;  // WRITE-ONLY (Env Reads)
    uint16_t shared_page_start;  // SHARED (Mediated)

    // Execution Context
    uint64_t pc;
    uint64_t sp; // Kept for convenience, actually R13
    std::vector<int64_t> regs; // R0-R15 (R13=SP, R15=PC mirrored generally but good to have full set)
    int64_t status;
    
    // Performance & Health Metrics
    int64_t health_score;    // 0-100
    int64_t stability_index; // Flux Metric
    int64_t learning_rate;   // Current plasticity factor

    Agent(uint32_t agent_id) 
        : id(agent_id), state(AgentState::INIT), priority(128), last_tick(0),
          belief_page_start(0), belief_page_count(0),
          input_page_start(0), output_page_start(0), shared_page_start(0),
          pc(0), sp(0), regs(16, 0), status(0),
          health_score(100), stability_index(0), learning_rate(0) {}
};

} // namespace Cognitive
} // namespace Helix
