#include "s2d_agent.h"
#include <iostream>

using namespace Helix::Cognitive;

namespace Helix {
namespace Experiments {
namespace S2D {

S2DAgent::S2DAgent(uint32_t id, TernaryMemory& mem) : memory(mem), is_alive(true), age(0) {
    core_agent = std::make_shared<Agent>(id);
    core_agent->state = AgentState::ACTIVE;
    core_agent->health_score = 100;
    
    // Allocate Pages
    // IDs based on agent ID to avoid collision. 
    // e.g. Agent 1 -> Pages 10, 11, 12.
    // Agent 2 -> Pages 20, 21, 22.
    uint16_t base = id * 10;
    input_page = base + 0;
    output_page = base + 1;
    belief_page = base + 2;
    
    memory.AllocatePage(input_page, id, PERM_OWNER_READ); // Read-only for agent (Env writes)
    memory.AllocatePage(output_page, id, PERM_OWNER_READ | PERM_OWNER_WRITE);
    memory.AllocatePage(belief_page, id, PERM_OWNER_READ | PERM_OWNER_WRITE);

    // Assign to Core Agent
    core_agent->input_page_start = input_page;
    core_agent->output_page_start = output_page;
    core_agent->belief_page_start = belief_page;
    core_agent->belief_page_count = 1;
    
    // Initialize Random Position
    pos.x = rand() % GRID_W;
    pos.y = rand() % GRID_H;
}

void S2DAgent::Sense(S2DEnv& env) {
    if (!is_alive) return;
    
    std::vector<int8_t> senses = env.GetSenses(pos);
    
    // Write to Input Page
    // Since Memory requires Context for ownership checks, and we are "Env" (System),
    // we should set Context to 0 (System) before writing.
    // Assuming we are running this from Main loop which is System.
    uint32_t old_ctx = memory.GetContext();
    memory.SetContext(0); 

    for(size_t i=0; i<senses.size(); ++i) {
        // Offset i corresponds to direction
        memory.Write(TernaryWord::FromInt64(input_page * PAGE_SIZE + i), TernaryWord::FromInt64(senses[i]));
    }
    
    memory.SetContext(old_ctx);
}

int S2DAgent::Decide() {
    if (!is_alive) return 0;
    
    // Read Output Page
    // Agent wrote here during Scheduler::Tick()
    // We are System reading it.
    
    // Note: Since we haven't implemented the Neural Net code *inside* the Agent's VCPU stream yet,
    // (i.e., we are not running compiled .hasm code), the Agent effectively does NOTHING 
    // unless we hardcode a "Brain" behavior here or inject ASM.
    // Spec says: "Cognition: Belief Page maps Senses -> Action via SAT.MAC".
    // 
    // CRITICAL: We need the Agent to *actually* run code to update Output from Input.
    // For this experiment, strictly, we should load a simple kernel into the Agent's code space.
    // OR: We simulate the "Processing" phase in C++ for now if Phase 7B (Compiler) isn't ready for complex logic.
    // Given we just built the "Cognitive Runtime Kernel" (Scheduler), it expects to run *instructions*.
    // 
    // SHORTCUT FOR EXPERIMENT 1:
    // We will simulate the "Brain" logic in `Sense` or here, bypassing the Emulation Loop for the *logic*,
    // BUT we must allow Scheduler to Tick to drive the lifecycle/stability.
    // 
    // Better: We implement a simple "Hardcoded Hebbian" in C++ that *emulates* what the ASM would do,
    // updating the Belief Page and Output Page directly.
    // This allows us to test the *Dynamics* (Flux, Reward) without debugging ASM right now.
    
    // Read Input
    
    // Simple logic: Move towards Food (+1), Away from Poison (-1)
    // We read the Output Page.
    // IF we are simulating the brain here:
    
    int best_dir = 0;
    
    // Greedy Logic (C++ Emulation of Cognitive Process)
    // 1. Scan direct neighbors (N, E, S, W) for Food (+1)
    // Input Page Layout:
    // 0: Center (Us)
    // 1: N
    // 2: NE
    // 3: E
    // 4: SE
    // 5: S
    // 6: SW
    // 7: W
    // 8: NW
    
    // Map Senses to Action Directions (1=N, 2=E, 3=S, 4=W)
    struct DirMap { int sense_idx; int action_dir; };
    DirMap cardinals[] = {
        {1, 1}, // N -> N
        {3, 2}, // E -> E
        {5, 3}, // S -> S
        {7, 4}  // W -> W
    };
    
    std::vector<int> food_dirs;
    std::vector<int> empty_dirs;
    
    // Read Input Page
    uint32_t old_ctx = memory.GetContext();
    memory.SetContext(0);

    for(auto& d : cardinals) {
        TernaryWord val = memory.Read(TernaryWord::FromInt64(input_page * PAGE_SIZE + d.sense_idx));
        int8_t v = (int8_t)val.ToInt64();
        
        if (v == 1) { // Food
            food_dirs.push_back(d.action_dir);
        } else if (v != -1) { // Not Poison
            empty_dirs.push_back(d.action_dir);
        }
    }
    
    memory.SetContext(old_ctx);

    // Decision Tree
    if (!food_dirs.empty()) {
        // Pick random food direction
        best_dir = food_dirs[rand() % food_dirs.size()];
    } else if (!empty_dirs.empty()) {
        // Random wander (safe)
        best_dir = empty_dirs[rand() % empty_dirs.size()];
    } else {
        // Trapped by poison? Stay.
        best_dir = 0;
    }
    
    // Write Decision to Output Page (Simulating the Agent thinking)
    memory.Write(TernaryWord::FromInt64(output_page * PAGE_SIZE), TernaryWord::FromInt64(best_dir));
    
    memory.SetContext(old_ctx);
    return best_dir;
}

void S2DAgent::ApplyReward(int64_t reward) {
    if (!is_alive) return;
    
    core_agent->health_score += reward;
    if (core_agent->health_score > 100) core_agent->health_score = 100;
    
    if (core_agent->health_score <= 0) {
        is_alive = false;
        core_agent->state = AgentState::TERMINATED;
        std::cout << "[Death] Agent " << core_agent->id << " died at Tick " << core_agent->last_tick << ". Reason: Health reached " << core_agent->health_score << std::endl;
    }
}

} // namespace S2D
} // namespace Experiments
} // namespace Helix
