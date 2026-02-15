#include "resonance_env.h"

namespace Helix {
namespace Experiments {
namespace Resonance {

ResonanceEnv::ResonanceEnv() : pattern_idx(0), noise_prob(0.3) {
    std::random_device rd;
    rng.seed(rd());
    // Default Pattern: +1, -1, +1
    pattern = {1, -1, 1};
}

void ResonanceEnv::SetNoiseLevel(double p) {
    noise_prob = p;
}

void ResonanceEnv::SetPattern(const std::vector<int8_t>& p) {
    pattern = p;
    pattern_idx = 0;
}

std::pair<int8_t, int8_t> ResonanceEnv::NextTick() {
    // 1. Get True Signal
    int8_t true_signal = pattern[pattern_idx];
    pattern_idx = (pattern_idx + 1) % pattern.size();
    
    // 2. Add Noise
    // Noise can flip trits or set to 0.
    // Simple noise: Replace with random trit w/ probability P
    std::uniform_real_distribution<> noise_dist(0.0, 1.0);
    std::uniform_int_distribution<> trit_dist(0, 2); // 0, 1, 2 maps to -1, 0, 1
    
    int8_t noisy_signal = true_signal;
    
    if (noise_dist(rng) < noise_prob) {
        // Generate random trit -1, 0, 1
        int r = trit_dist(rng);
        if (r == 0) noisy_signal = -1;
        else if (r == 1) noisy_signal = 0;
        else noisy_signal = 1;
    }
    
    return {noisy_signal, true_signal};
}

} // namespace Resonance
} // namespace Experiments
} // namespace Helix
