#include "stability_monitor.h"
#include <cmath>
#include <numeric>

namespace Helix {
namespace Cognitive {

StabilityMonitor::StabilityMonitor() : window_size(10), flux_threshold(0.01) {}

void StabilityMonitor::SetWindowSize(int ticks) {
    window_size = ticks;
}

void StabilityMonitor::SetFluxThreshold(double threshold) {
    flux_threshold = threshold;
}

void StabilityMonitor::CaptureState(const Agent& agent, TernaryMemory& mem) {
    // 1. Snapshot Belief Page
    if (mem.IsPageAllocated(agent.belief_page_start)) {
        // We need a deep copy of the page
        // Since Memory doesn't expose raw pointer easily, we read words.
        // Actually, we can use the friend access or just copy words if we add a Helper.
        // For now, let's assume we can get the page or read it word-by-word.
        // Optimization: Memory needs a GetPageCopy method?
        // Or we just iterate.
        
        Page current_page;
        for(int i=0; i<PAGE_SIZE; ++i) {
            current_page.words[i] = mem.Read(TernaryWord::FromInt64(agent.belief_page_start * PAGE_SIZE + i));
        }

        // 2. Calculate Flux vs Previous
        if (last_belief_state.count(agent.id)) {
            int64_t raw_flux = CalculateFlux(last_belief_state[agent.id], current_page);
            double normalized_flux = (double)raw_flux / (PAGE_SIZE * 2); // Max change is 2 per trit? No, typical hamming
            // Trit distance: 0->1 is 1. -1->1 is 2.
            // Let's normalize by max possible change (2 * 256).

            histories[agent.id].flux_history.push_back(normalized_flux);
            if (histories[agent.id].flux_history.size() > window_size) {
                histories[agent.id].flux_history.pop_front();
            }
        }
        
        // 3. Update Snapshot
        last_belief_state[agent.id] = current_page;
    }
}

StabilityMetrics StabilityMonitor::Analyze(uint32_t agent_id) {
    StabilityMetrics m = {0,0,0, false};
    if (histories.count(agent_id)) {
        auto& h = histories[agent_id];
        
        if (!h.flux_history.empty()) {
            double sum = std::accumulate(h.flux_history.begin(), h.flux_history.end(), 0.0);
            m.flux = sum / h.flux_history.size();
        }
        
        m.converged = (m.flux < flux_threshold);
    }
    return m;
}

int64_t StabilityMonitor::CalculateFlux(const Page& p1, const Page& p2) {
    int64_t flux = 0;
    for (int i=0; i<PAGE_SIZE; ++i) {
        int64_t v1 = p1.words[i].ToInt64();
        int64_t v2 = p2.words[i].ToInt64();
        flux += std::abs(v1 - v2);
    }
    return flux;
}

} // namespace Cognitive
} // namespace Helix
