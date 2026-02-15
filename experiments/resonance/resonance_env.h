#pragma once
#include <vector>
#include <cstdint>
#include <random>

namespace Helix {
namespace Experiments {
namespace Resonance {

class ResonanceEnv {
public:
    ResonanceEnv();

    // Configuration
    void SetNoiseLevel(double noise_prob); // 0.0 to 1.0
    void SetPattern(const std::vector<int8_t>& pattern);

    // Simulation
    // Returns the next input signal (Pattern + Noise)
    // Also returns the "True" signal for scoring
    std::pair<int8_t, int8_t> NextTick();

    int GetPatternLength() const { return pattern.size(); }

private:
    std::vector<int8_t> pattern;
    int pattern_idx;
    double noise_prob;
    std::mt19937 rng;
};

} // namespace Resonance
} // namespace Experiments
} // namespace Helix
