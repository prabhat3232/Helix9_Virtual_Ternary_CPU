#pragma once
#include <vector>
#include <cstdint>
#include <random>
#include <map>

namespace Helix {
namespace Experiments {
namespace S2D {

// Grid Constants
const int GRID_W = 32;
const int GRID_H = 32;

enum class CellType : int8_t {
    EMPTY = 0,
    FOOD = 1,
    POISON = -1,
    AGENT = 2 // Visual only, Agents tracked separately
};

struct Position {
    int x, y;
    bool operator==(const Position& other) const { return x == other.x && y == other.y; }
};

class S2DEnv {
public:
    S2DEnv();

    void Initialize();
    void Update(); // Spawn items, cleanup
    
    // Agent Interaction
    // Returns 9 ternary values (Center + 8 neighbors)
    std::vector<int8_t> GetSenses(Position pos);
    
    // Returns true if move valid (not wall, though toroidal has no walls)
    // Returns reward/penalty if hit something
    int8_t TryMove(uint32_t agent_id, Position& current_pos, int direction);

    // Render helper (returns ASCII string)
    std::string Render();

    // State Access
    CellType GetCell(int x, int y) const;
    void SetCell(int x, int y, CellType type);
    
    // Global Stats
    int food_count;
    int poison_count;

private:
    CellType grid[GRID_H][GRID_W];
    std::mt19937 rng;
    
    // Config
    double food_spawn_rate;
    double poison_spawn_rate;
};

} // namespace S2D
} // namespace Experiments
} // namespace Helix
