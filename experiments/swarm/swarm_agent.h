#pragma once
#include "../../src/cognitive/agent.h"
#include "../../src/memory.h"
#include "swarm_env.h"
#include <memory>

namespace Helix {
namespace Experiments {
namespace Swarm {

class SwarmAgent {
public:
    SwarmAgent(uint32_t id, TernaryMemory& mem);

    void Sense(SwarmEnv& env);
    int Decide(); // Returns direction
    
    std::shared_ptr<Helix::Cognitive::Agent> core_agent;
    Position pos;
    int last_action;

private:
    TernaryMemory& memory;
    uint16_t input_page;
    uint16_t output_page;
    uint16_t belief_page; // Store current preferred direction vector?
};

} // namespace Swarm
} // namespace Experiments
} // namespace Helix
