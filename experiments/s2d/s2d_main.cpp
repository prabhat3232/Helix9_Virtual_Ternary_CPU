#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "../../src/cpu.h"
#include "../../src/cognitive/scheduler.h"
#include "s2d_env.h"
#include "s2d_agent.h"

using namespace Helix;
using namespace Helix::Cognitive;
using namespace Helix::Experiments::S2D;

const int NUM_AGENTS = 5;
const int SIM_TICKS = 1000;

int main() {
    std::cout << "Helix9 2D Seeker Experiment" << std::endl;
    std::cout << "---------------------------" << std::endl;

    // 1. Setup Kernel
    TernaryMemory mem;
    Cpu cpu(mem);
    Scheduler scheduler(cpu);
    
    // 2. Setup Env
    S2DEnv env;
    env.Initialize();

    // 3. Spawn Agents
    std::vector<std::unique_ptr<S2DAgent>> agents;
    for(int i=1; i<=NUM_AGENTS; ++i) {
        auto ag = std::make_unique<S2DAgent>(i, mem);
        scheduler.RegisterAgent(ag->core_agent);
        agents.push_back(std::move(ag));
        std::cout << "Spawned Agent " << i << " at (" << agents.back()->pos.x << "," << agents.back()->pos.y << ")" << std::endl;
    }

    // 4. Run Loop
    for(int tick=0; tick<SIM_TICKS; ++tick) {
        // Env Update (Spawn Items)
        env.Update();
        
        // Sense & Visual Update
        // Clear old agent positions from grid for rendering
        for(int y=0; y<GRID_H; ++y) {
            for(int x=0; x<GRID_W; ++x) {
                if (env.GetCell(x,y) == CellType::AGENT) env.SetCell(x,y, CellType::EMPTY);
            }
        }

        int alive_count = 0;
        
        for(auto& ag : agents) {
            if (!ag->is_alive) continue;
            alive_count++;
            
            // Render Agent
            env.SetCell(ag->pos.x, ag->pos.y, CellType::AGENT);
            
            // Sense
            ag->Sense(env);
            
            // Brain (Placeholder logic in simple C++ wrapper for now)
            int dir = ag->Decide();
            
            // Act
            int8_t reward = env.TryMove(ag->core_agent->id, ag->pos, dir);
            
            // Reward
            // Hunger decay
            reward -= 1; 
            ag->ApplyReward(reward);
        }

        // Kernel Tick (Advances Cognitive Clock, Updates Stability)
        scheduler.Tick();
        
        if (tick % 100 == 0) {
            int total_health = 0;
            for(auto& ag : agents) if(ag->is_alive) total_health += ag->core_agent->health_score;
            int avg_health = alive_count > 0 ? total_health / alive_count : 0;

            std::cout << "Tick " << tick << " | Agents Alive: " << alive_count 
                      << " | Avg Health: " << avg_health
                      << " | Food: " << env.food_count 
                      << " | Poison: " << env.poison_count << std::endl;
            // std::cout << env.Render() << std::endl; // Too spammy for non-interactive
        }

        if (alive_count == 0) {
            std::cout << "Extinction event at Tick " << tick << std::endl;
            break;
        }
        
        // Sleep for viz?
        // std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    std::cout << "Simulation Complete." << std::endl;
    return 0;
}
