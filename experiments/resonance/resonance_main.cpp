#include <iostream>
#include <iomanip>
#include <vector>
#include "../../src/cpu.h"
#include "resonance_env.h"
#include "resonance_agent.h"

using namespace Helix;
using namespace Helix::Experiments::Resonance;

const int TICKS = 200;

int main() {
    std::cout << "Helix9 Pattern Locking Experiment" << std::endl;
    std::cout << "---------------------------------" << std::endl;
    std::cout << "Target Pattern: [+1, -1, +1]" << std::endl;
    std::cout << "Noise Level: 30%" << std::endl;
    std::cout << "Heuristic: Internal Phase Locking (matches period 3)" << std::endl;

    TernaryMemory mem;
    ResonanceEnv env;
    env.SetNoiseLevel(0.3);
    
    // Pattern Length 3
    ResonanceAgent agent(1, mem, 3);
    
    int correct_ticks = 0;
    int streak = 0;

    for(int tick=0; tick<TICKS; ++tick) {
        // 1. Env Tick
        auto [noisy_sig, true_sig] = env.NextTick();
        
        // 2. Sense
        agent.Sense(noisy_sig);
        
        // 3. Decide (based on internal weights)
        int8_t output = agent.Decide();
        
        // 4. Learn (Feedback)
        agent.Learn(true_sig); // Supervised for this exp? Or Reward based?
        // Spec says: "If Action == Target: Reward".
        // In Learn(), we use true_sig to simulate the Reward signal provided by the Env.
        
        // Metrics
        bool match = (output == true_sig);
        if (match) {
            correct_ticks++;
            streak++;
        } else {
            streak = 0;
        }
        
        if (tick % 20 == 0 || tick == TICKS-1) {
            std::cout << "Tick " << std::setw(3) << tick 
                      << " | Weights: [";
            for(int w : agent.weights) std::cout << std::setw(3) << w << " ";
            std::cout << "]"
                      << " | Lock Streak: " << streak 
                      << " | Output: " << (int)output << " (vs " << (int)true_sig << ")"
                      << std::endl;
        }
    }
    
    std::cout << "\nAccuracy: " << (double)correct_ticks / TICKS * 100.0 << "%" << std::endl;
    return 0;
}
