#include "swarm_env.h"
#include <cstring>

namespace Helix {
namespace Experiments {
namespace Swarm {

SwarmEnv::SwarmEnv() {
    Initialize();
}

void SwarmEnv::Initialize() {
    std::memset(grid, 0, sizeof(grid));
    agent_actions.clear();
}

void SwarmEnv::UpdateAgentPos(uint32_t id, Position pos) {
    // Clear old? We don't track old pos here easily without context.
    // Assume caller clears old or we rebuild grid every tick.
    // Let's adopt "Rebuild Grid" approach in Main for simplicity.
    int wx = (pos.x + GRID_W) % GRID_W;
    int wy = (pos.y + GRID_H) % GRID_H;
    grid[wy][wx] = id;
}

void SwarmEnv::UpdateAgentAction(uint32_t id, int action) {
    agent_actions[id] = action;
}

std::vector<int> SwarmEnv::GetNeighborActions(Position pos) {
    std::vector<int> actions;
    // N, NE, E, SE, S, SW, W, NW
    int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
    int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};

    for(int i=0; i<8; ++i) {
        int nx = (pos.x + dx[i] + GRID_W) % GRID_W;
        int ny = (pos.y + dy[i] + GRID_H) % GRID_H;
        
        uint32_t neighbor_id = grid[ny][nx];
        if (neighbor_id != 0) {
            // Found neighbor
            if (agent_actions.count(neighbor_id)) {
                actions.push_back(agent_actions[neighbor_id]);
            } else {
                actions.push_back(0); // Default/Still
            }
        } else {
            actions.push_back(-1); // Empty
        }
    }
    return actions;
}

Position SwarmEnv::CalculateMove(Position current, int direction) {
    int dx[] = {0, 0, 1, 0, -1};
    int dy[] = {0, -1, 0, 1, 0};
    
    if (direction < 0 || direction > 4) direction = 0;
    
    Position next;
    next.x = (current.x + dx[direction] + GRID_W) % GRID_W;
    next.y = (current.y + dy[direction] + GRID_H) % GRID_H;
    return next;
}

} // namespace Swarm
} // namespace Experiments
} // namespace Helix
