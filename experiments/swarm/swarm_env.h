#pragma once
#include <vector>
#include <cstdint>
#include <map>
#include <string>

namespace Helix {
namespace Experiments {
namespace Swarm {

const int GRID_W = 32;
const int GRID_H = 32;

struct Position {
    int x, y;
};

class SwarmEnv {
public:
    SwarmEnv();
    void Initialize();
    
    // Agent Registry (for Neighbor lookup)
    void UpdateAgentPos(uint32_t id, Position pos);
    void UpdateAgentAction(uint32_t id, int action);
    
    // Sensing
    // Returns list of Neighbor Actions (0-4) for the 8 neighbors.
    // -1 if empty.
    std::vector<int> GetNeighborActions(Position pos);

    // Physics
    // Simple move, no collision blocking for swarm (ghost mode) or blocking?
    // Let's allow overlapping to focus on alignment.
    Position CalculateMove(Position current, int direction);

private:
    // Grid stores Agent IDs (0 if empty)
    // To handle multiple agents on one cell, we might need a list, 
    // but for simple swarm, let's assume one main occupant or just lookups.
    // Better: Grid stores "Count" or "Lead Agent".
    // 
    // Actually, simplest is to iterate all agents to find neighbors? 
    // No, O(N^2). Grid is better O(1).
    // Let's just store the ID of *an* agent at (x,y).
    uint32_t grid[GRID_H][GRID_W]; // 0 = Empty

    // Map ID -> Last Action
    std::map<uint32_t, int> agent_actions;
};

} // namespace Swarm
} // namespace Experiments
} // namespace Helix
