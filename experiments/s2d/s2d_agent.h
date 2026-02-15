#pragma once
#include "../../src/cognitive/agent.h"
#include "../../src/memory.h"
#include "s2d_env.h"
#include <memory>

namespace Helix {
namespace Experiments {
namespace S2D {

class S2DAgent {
public:
    S2DAgent(uint32_t id, TernaryMemory& mem);

    // Interface
    void Sense(S2DEnv& env);
    int Decide(); // Returns direction 0-4
    void ApplyReward(int64_t reward);

    // Public for Main loop access
    std::shared_ptr<Helix::Cognitive::Agent> core_agent;
    Position pos;
    bool is_alive;
    int age;

private:
    TernaryMemory& memory;
    
    // Page IDs
    uint16_t input_page;
    uint16_t output_page;
    uint16_t belief_page;
};

} // namespace S2D
} // namespace Experiments
} // namespace Helix
