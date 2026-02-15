#include "swarm_agent.h"
#include <iostream>
#include <random>

using namespace Helix::Cognitive;

namespace Helix {
namespace Experiments {
namespace Swarm {

SwarmAgent::SwarmAgent(uint32_t id, TernaryMemory& mem) : memory(mem), last_action(0) {
    core_agent = std::make_shared<Agent>(id);
    core_agent->state = AgentState::ACTIVE;
    core_agent->health_score = 100;

    uint16_t base = id * 10;
    input_page = base + 0;
    output_page = base + 1;
    belief_page = base + 2;

    memory.AllocatePage(input_page, id, PERM_OWNER_READ);
    memory.AllocatePage(output_page, id, PERM_OWNER_READ | PERM_OWNER_WRITE);
    memory.AllocatePage(belief_page, id, PERM_OWNER_READ | PERM_OWNER_WRITE);

    // Random Start Pos
    pos.x = rand() % GRID_W;
    pos.y = rand() % GRID_H;
    
    // Random Start Action/Belief
    last_action = (rand() % 4) + 1; // 1-4
}

void SwarmAgent::Sense(SwarmEnv& env) {
    std::vector<int> neighbors = env.GetNeighborActions(pos);
    
    uint32_t old_ctx = memory.GetContext();
    memory.SetContext(0);

    for(size_t i=0; i<neighbors.size(); ++i) {
        // Write neighbor actions to Input Page [0..7]
        // Mapping: -1 (Empty) -> 0? Or -1?
        // Let's use:
        // 0: Empty/Still
        // 1: N, 2: E, 3: S, 4: W
        int val = neighbors[i];
        if (val == -1) val = 0; 
        
        memory.Write(TernaryWord::FromInt64(input_page * PAGE_SIZE + i), TernaryWord::FromInt64(val));
    }

    memory.SetContext(old_ctx);
}

int SwarmAgent::Decide() {
    // Alignment Logic:
    // 1. Read Neighbor Actions from Input Page.
    // 2. Compute Vector Sum or Majority Vote.
    // 3. Update Own Action.
    
    uint32_t old_ctx = memory.GetContext();
    memory.SetContext(0); // System impersonating Agent Brain

    int counts[5] = {0}; // 0..4
    
    // Self Bias (Inertia) - Weight 2
    counts[last_action] += 2;

    for(int i=0; i<8; ++i) {
        TernaryWord w = memory.Read(TernaryWord::FromInt64(input_page * PAGE_SIZE + i));
        int act = (int)w.ToInt64();
        if (act >= 0 && act <= 4) {
            if (act != 0) counts[act]++;
        }
    }
    
    memory.SetContext(old_ctx);

    // Find Max Vote
    int best_act = 0;
    int max_votes = -1;
    
    std::vector<int> candidates;
    for(int i=1; i<=4; ++i) {
        if (counts[i] > max_votes) {
            max_votes = counts[i];
            candidates.clear();
            candidates.push_back(i);
        } else if (counts[i] == max_votes) {
            candidates.push_back(i);
        }
    }
    
    if (!candidates.empty()) {
        best_act = candidates[rand() % candidates.size()];
    } else {
        best_act = last_action;
    }

    last_action = best_act;
    return best_act;
}

} // namespace Swarm
} // namespace Experiments
} // namespace Helix
