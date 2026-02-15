#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>
#include <iomanip>
#include "../../src/cpu.h"
#include "../../src/cognitive/scheduler.h"
#include "swarm_env.h"
#include "swarm_agent.h"

using namespace Helix;
using namespace Helix::Experiments::Swarm;

const int NUM_AGENTS = 20;
const int TICKS = 500;

double CalculateAlignment(const std::vector<std::unique_ptr<SwarmAgent>>& agents) {
    // Simple metric: Count most common direction pct
    int counts[5] = {0};
    int total_moving = 0;
    
    for(auto& ag : agents) {
        if (ag->last_action != 0) {
            counts[ag->last_action]++;
            total_moving++;
        }
    }
    
    if (total_moving == 0) return 0.0;
    
    int max_c = 0;
    for(int i=1; i<=4; ++i) if(counts[i] > max_c) max_c = counts[i];
    
    return (double)max_c / total_moving;
}

int main() {
    std::cout << "Helix9 Multi-Agent Swarm Experiment" << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    TernaryMemory mem;
    SwarmEnv env;
    std::vector<std::unique_ptr<SwarmAgent>> agents;

    // Spawn
    for(int i=1; i<=NUM_AGENTS; ++i) {
        agents.push_back(std::make_unique<SwarmAgent>(i, mem));
    }

    // Loop
    for(int tick=0; tick<TICKS; ++tick) {
        // 1. Rebuild Env Grid
        env.Initialize(); // Clears grid
        for(auto& ag : agents) {
            env.UpdateAgentPos(ag->core_agent->id, ag->pos);
            env.UpdateAgentAction(ag->core_agent->id, ag->last_action);
        }
        
        // 2. Agents Think
        for(auto& ag : agents) {
            ag->Sense(env);
            ag->Decide(); // Updates last_action
        }
        
        // 3. Agents Move
        for(auto& ag : agents) {
            if (ag->last_action != 0) {
                ag->pos = env.CalculateMove(ag->pos, ag->last_action);
            }
        }
        
        // 4. Metrics
        double alignment = CalculateAlignment(agents);
        
        if (tick % 50 == 0) {
            int total_neighbors = 0;
            for(auto& ag : agents) {
                // Peek env for neighbor count
                std::vector<int> n = env.GetNeighborActions(ag->pos);
                for(int a : n) if(a != -1) total_neighbors++;
            }
            double avg_neighbors = (double)total_neighbors / NUM_AGENTS;
            
            std::cout << "Tick " << std::setw(3) << tick 
                      << " | Alignment: " << std::fixed << std::setprecision(2) << alignment * 100.0 << "%"
                      << " | Avg Neighbors: " << avg_neighbors
                      << " | ";
            
            // Print distribution
             int counts[5] = {0};
             for(auto& ag : agents) counts[ag->last_action]++;
             std::cout << "N:" << counts[1] << " E:" << counts[2] << " S:" << counts[3] << " W:" << counts[4];
             std::cout << std::endl;
        }
    }
    
    // Final Visualization
    std::cout << "\nFinal Swarm State:\n";
    char sym[] = {' ', '^', '>', 'v', '<'};
    for(int y=0; y<GRID_H; ++y) {
        for(int x=0; x<GRID_W; ++x) {
            bool found = false;
            for(auto& ag : agents) {
                if(ag->pos.x == x && ag->pos.y == y) {
                    std::cout << sym[ag->last_action];
                    found = true;
                    break;
                }
            }
            if(!found) std::cout << ".";
        }
        std::cout << "\n";
    }

    return 0;
}
