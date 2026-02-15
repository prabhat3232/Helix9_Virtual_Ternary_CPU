#pragma once
#include <vector>
#include <deque>
#include "../memory.h"
#include "agent.h"

namespace Helix {
namespace Cognitive {

struct StabilityMetrics {
    double flux;          // Normalized rate of change (0.0 - 1.0)
    double energy;        // Normalized energy consumption
    double coherence;     // Consensus alignment
    bool converged;       // True if flux < threshold for N ticks
};

class StabilityMonitor {
public:
    StabilityMonitor();

    // Configuration
    void SetWindowSize(int ticks);
    void SetFluxThreshold(double threshold);

    // Runtime
    void CaptureState(const Agent& agent, TernaryMemory& mem);
    StabilityMetrics Analyze(uint32_t agent_id);

    // Helpers
    // Calculate Ternary Flux between two pages (Hamming Distance)
    static int64_t CalculateFlux(const Page& p1, const Page& p2);

private:
    int window_size;
    double flux_threshold;
    
    // Agent ID -> History of Metrics
    struct AgentHistory {
        std::deque<double> flux_history;
        std::deque<double> energy_history;
    };
    
    std::unordered_map<uint32_t, AgentHistory> histories;
    // Agent ID -> Previous Belief Page Snapshot
    std::unordered_map<uint32_t, Page> last_belief_state;
};

} // namespace Cognitive
} // namespace Helix
