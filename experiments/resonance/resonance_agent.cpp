#include "resonance_agent.h"
#include <iostream>
#include <numeric>

using namespace Helix::Cognitive;

namespace Helix {
namespace Experiments {
namespace Resonance {

ResonanceAgent::ResonanceAgent(uint32_t id, TernaryMemory& mem, int plen) 
    : memory(mem), pattern_len(plen) {
    
    core_agent = std::make_shared<Agent>(id);
    input_page = id * 10;
    memory.AllocatePage(input_page, id, PERM_OWNER_READ | PERM_OWNER_WRITE);
    
    // Initialize Weights to 0 (Tabula Rasa)
    // We need 'plen' weights to match history positions? 
    // Actually, for a *temporal* sequence pattern lock, we usually match 
    // "Current State" -> "Next Prediction".
    // 
    // But here the spec says: "Weights: Vector of length L".
    // "Activation: Sum(Buffer * Weights)".
    // This implies a spatial filter over the history buffer to detect the pattern *occurrence*.
    // 
    // Let's implement a simple predictor:
    // If we see [A, B], is C likely?
    // 
    // Simplified Resonance: 
    // We just maintain a spinning buffer of weights that hopefully aligns with the pattern phase.
    // 
    // Let's stick to the spec: "Locked" means output matches signal.
    // We will use a standard Hebbian Weight vector of size 'plen'.
    // Weights[i] corresponds to Pattern[i].
    // But since the agent doesn't know "absolute time", it needs to just resonate.
    // 
    // Simplest temporal lock: 
    // Internal Oscillator of length L.
    // Weights[t] += Signal.
    
    weights.resize(plen, 0);
    for(int i=0; i<plen; ++i) history.push_back(0);
}

void ResonanceAgent::Sense(int8_t signal) {
    history.push_back(signal);
    if (history.size() > pattern_len) history.pop_front();
    
    // Write to memory for completeness
    memory.Write(TernaryWord::FromInt64(input_page * PAGE_SIZE), TernaryWord::FromInt64(signal));
}

int8_t ResonanceAgent::Decide() {
    // We are trying to *predict* or *match* the signal?
    // "Lock on": Output should match Signal *Phase*.
    // 
    // Let's try to output based on the weights at the current internal phase.
    // But we don't know the phase locally.
    // 
    // Alternative Spec Interpretation:
    // "Activation = Sum(Buffer * Weights)"
    // This is a matched filter.
    // If Buffer matches Pattern (weights), Activation is High (+1).
    // If Buffer matches Anti-Pattern, Low (-1).
    // 
    // Let's output the Activation Sign.
    // If the input stream is matching our learned weights, we output +1.
    // 
    // BUT the goal "Lock On" usually means generating the *next* token or synchronized token.
    // Let's assume the Agent tries to Output = Signal (Resonate).
    // 
    // Strategy:
    // Weights[Phase] stores the expected value at this phase.
    // We iterate Phase 0..L-1 cyclically.
    // return Sign(Weights[Phase]).
    
    static int phase = 0;
    int w = weights[phase];
    phase = (phase + 1) % pattern_len;
    
    if (w > 5) return 1;
    if (w < -5) return -1;
    return 0; // Not sure
}

void ResonanceAgent::Learn(int8_t true_signal) {
    // Get the weight index corresponding to the *previous* output decision?
    // We need to update the weight that *produced* the decision.
    // 
    static int learn_phase = 0; // Distinct form Decide phase? 
    // Actually, Learn happens after Decide in the same tick.
    // So current learn_phase = (decide_phase - 1).
    
    // Let's simplify: 
    // Agent has internal global phase counter relative to its start?
    // No.
    // 
    // Let's implement independent Hebbian Learning on the weights.
    // Weights[current_step % L] += true_signal.
    // This assumes the Agent has a fixed internal clock period L.
    // If the external signal also has period L, they will drift until they align (beat frequency),
    // or we assume they are same frequency, just phase shifted.
    
    // We assume Period matches.
    static int step = 0;
    int idx = step % pattern_len;
    
    // Hebbian Update: W_new = W_old + LearningRate * Signal
    if (true_signal == 1) weights[idx] += 2;
    if (true_signal == -1) weights[idx] -= 2;
    
    // Decay/Clamp
    if (weights[idx] > 50) weights[idx] = 50;
    if (weights[idx] < -50) weights[idx] = -50;
    
    // "Forget" noise (Leak)
    if (true_signal == 0) {
        if (weights[idx] > 0) weights[idx]--;
        if (weights[idx] < 0) weights[idx]++;
    }
    
    step++;
}

} // namespace Resonance
} // namespace Experiments
} // namespace Helix
