#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include "agent.h"

namespace Helix {
namespace Cognitive {

struct RewardConfig {
    int64_t max_reward = 100;
    int64_t min_reward = -100;
    double decay_factor = 0.95; // Smoothing
    double learning_rate_base = 0.1;
};

class RewardEngine {
public:
    RewardEngine();
    
    // Configuration
    void SetConfig(const RewardConfig& config);
    
    // Core Logic
    // Takes raw environmental signal, normalizes it, and updates Agent state
    void ApplyReward(Agent& agent, int64_t raw_signal);

private:
    RewardConfig config;
    
    // Helper: Sigmoid or Clamp?
    // We use Clamping for v1.0 Research Grade as per Spec.
    int64_t Normalize(int64_t raw);
};

} // namespace Cognitive
} // namespace Helix
