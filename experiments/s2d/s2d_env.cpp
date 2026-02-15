#include "s2d_env.h"
#include <iostream>
#include <sstream>

namespace Helix {
namespace Experiments {
namespace S2D {

S2DEnv::S2DEnv() : food_count(0), poison_count(0), food_spawn_rate(0.8), poison_spawn_rate(0.1) {
    std::random_device rd;
    rng.seed(rd());
}

void S2DEnv::Initialize() {
    for(int y=0; y<GRID_H; ++y) {
        for(int x=0; x<GRID_W; ++x) {
            grid[y][x] = CellType::EMPTY;
        }
    }
    
    // Initial Seed
    std::uniform_int_distribution<> grid_dist(0, GRID_W - 1);
    for(int i=0; i<20; ++i) {
        int x = grid_dist(rng);
        int y = grid_dist(rng);
        if (grid[y][x] == CellType::EMPTY) {
            grid[y][x] = CellType::FOOD;
            food_count++;
        }
    }
}

void S2DEnv::Update() {
    std::uniform_real_distribution<> dist(0.0, 1.0);
    std::uniform_int_distribution<> grid_dist(0, GRID_W - 1);

    // Spawn Food
    if (dist(rng) < food_spawn_rate) {
        int x = grid_dist(rng);
        int y = grid_dist(rng);
        if (grid[y][x] == CellType::EMPTY) {
            grid[y][x] = CellType::FOOD;
            food_count++;
        }
    }

    // Spawn Poison
    if (dist(rng) < poison_spawn_rate) {
        int x = grid_dist(rng);
        int y = grid_dist(rng);
        if (grid[y][x] == CellType::EMPTY) {
            grid[y][x] = CellType::POISON;
            poison_count++;
        }
    }
}

CellType S2DEnv::GetCell(int x, int y) const {
    // Toroidal wrap
    int wx = (x + GRID_W) % GRID_W;
    int wy = (y + GRID_H) % GRID_H;
    return grid[wy][wx];
}

void S2DEnv::SetCell(int x, int y, CellType type) {
    int wx = (x + GRID_W) % GRID_W;
    int wy = (y + GRID_H) % GRID_H;
    grid[wy][wx] = type;
}

std::vector<int8_t> S2DEnv::GetSenses(Position pos) {
    std::vector<int8_t> senses;
    // Order: Center, N, NE, E, SE, S, SW, W, NW
    // Standard Moore Neighborhood
    int dx[] = {0, 0, 1, 1, 1, 0, -1, -1, -1};
    int dy[] = {0, -1, -1, 0, 1, 1, 1, 0, -1};

    for(int i=0; i<9; ++i) {
        CellType c = GetCell(pos.x + dx[i], pos.y + dy[i]);
        senses.push_back((int8_t)c);
    }
    return senses;
}

int8_t S2DEnv::TryMove(uint32_t agent_id, Position& current_pos, int direction) {
    // Dir: 0=Stay, 1=N, 2=E, 3=S, 4=W
    int dx[] = {0, 0, 1, 0, -1};
    int dy[] = {0, -1, 0, 1, 0};

    if (direction < 0 || direction > 4) direction = 0;

    int new_x = current_pos.x + dx[direction];
    int new_y = current_pos.y + dy[direction];
    
    // Wrap
    new_x = (new_x + GRID_W) % GRID_W;
    new_y = (new_y + GRID_H) % GRID_H;

    CellType target = grid[new_y][new_x];
    int8_t reward = 0;

    if (target == CellType::FOOD) {
        reward = 10;
        grid[new_y][new_x] = CellType::EMPTY; // Consumed
        food_count--;
    } else if (target == CellType::POISON) {
        reward = -10;
        grid[new_y][new_x] = CellType::EMPTY; // Consumed/Destroyed
        poison_count--;
    } else if (target == CellType::AGENT) {
        // Collision with another agent?
        // For now, allow overlap but maybe penalty?
        reward = -1; 
    }

    current_pos.x = new_x;
    current_pos.y = new_y;
    return reward;
}

std::string S2DEnv::Render() {
    std::stringstream ss;
    ss << "+";
    for(int x=0; x<GRID_W; ++x) ss << "-";
    ss << "+\n";

    for(int y=0; y<GRID_H; ++y) {
        ss << "|";
        for(int x=0; x<GRID_W; ++x) {
            switch(grid[y][x]) {
                case CellType::EMPTY: ss << " "; break;
                case CellType::FOOD: ss << "."; break; // Green normally
                case CellType::POISON: ss << "x"; break; // Red
                case CellType::AGENT: ss << "A"; break;
            }
        }
        ss << "|\n";
    }
    ss << "+";
    for(int x=0; x<GRID_W; ++x) ss << "-";
    ss << "+";
    return ss.str();
}

} // namespace S2D
} // namespace Experiments
} // namespace Helix
