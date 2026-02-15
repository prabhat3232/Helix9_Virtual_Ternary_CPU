#pragma once
#include "../../src/cognitive/agent.h"
#include "../../src/memory.h"
#include <vector>
#include <deque>

namespace Helix {
namespace Experiments {
namespace Resonance {

class ResonanceAgent {
public:
    ResonanceAgent(uint32_t id, TernaryMemory& mem, int pattern_len);

    // Cycle
    void Sense(int8_t signal);
    int8_t Decide(); // Returns prediction (-1, 0, +1)
    void Learn(int8_t true_signal); // Apply Reward/Penalty

    std::shared_ptr<Helix::Cognitive::Agent> core_agent;
    
    // Debug Access
    std::vector<int> weights; // Using int for simpler weight tracking (-100 to 100)

private:
    TernaryMemory& memory;
    uint16_t input_page;
    std::deque<int8_t> history; // Simple buffer
    int pattern_len;
};

} // namespace Resonance
} // namespace Experiments
} // namespace Helix
