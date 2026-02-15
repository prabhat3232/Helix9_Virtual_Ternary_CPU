#include "reward_engine.h"
#include <algorithm>
#include <cmath>

namespace Helix {
namespace Cognitive {

RewardEngine::RewardEngine() {}

void RewardEngine::SetConfig(const RewardConfig& cfg) {
    config = cfg;
}

void RewardEngine::ApplyReward(Agent& agent, int64_t raw_signal) {
    if (agent.state != AgentState::LEARNING) return;

    // 1. Normalize
    int64_t r = Normalize(raw_signal);
    
    // 2. Adjust Learning Rate / Plasticity
    // If Reward is High (positive or negative magnitude), increase plasticity temporarily?
    // Or: Reward dictates direction.
    // In Helix, "Reward" might be injected into a specific "Dopamine Register" or Page.
    // For now, valid Research-Grade approach:
    // Update Agent's internal "Subjective Utility" or Health.
    
    agent.health_score += (r / 10); // Simple accumulation
    if (agent.health_score > 100) agent.health_score = 100;
    if (agent.health_score < 0) agent.health_score = 0;
    
    // 3. Modulate Learning Rate
    // If flux is low (converged) but reward is negative -> Increase Learning Rate (Wake up)
    // If flux is high (chaos) -> Decrease Learning Rate (Cool down)
    // This connects Stability to Reward.
    // (This logic might belong in Scheduler, but Engine provides the tools)
}

int64_t RewardEngine::Normalize(int64_t raw) {
    // Hard Clamp
    if (raw > config.max_reward) return config.max_reward;
    if (raw < config.min_reward) return config.min_reward;
    return raw;
}

} // namespace Cognitive
} // namespace Helix
