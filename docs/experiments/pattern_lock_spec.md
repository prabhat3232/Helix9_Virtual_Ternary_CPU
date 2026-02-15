# Experiment 3: Pattern Locking (Resonance)

## Overview
This experiment validates the **Stability Monitor** (`Flux`) and **Reward Engine** (`Energy`) by training an Agent to "Lock On" to a specific 3-trit temporal pattern (e.g., `+1, -1, +1`) hidden within a noisy stream.

## Hypothesis
An agent using **Hebbian Learning** (Reinforcing weights on Reward) + **Stability Decay** (Reducing weights on high Flux) can filter out uncorrelated noise and resonate only with the target signal.

## Architecture

### 1. Environment (`resonance_env.cpp`)
*   **Signal Source**:Emits a repeating Pattern P (Length L=3).
*   **Noise Source**: Emits random trits.
*   **Channel**: `Input = (Signal + Noise) > Threshold ? Signal : Noise`. 
    *   (Simplified: 70% Signal, 30% Random).

### 2. Agent (`resonance_agent.cpp`)
*   **Input Page**: 
    *   Offset 0: Current Input Signal.
*   **Internal State (Brain)**:
    *   **Weights**: Vector of length L (3 words).
    *   **Buffer**: History of last 3 inputs.
*   **Cognitive Logic**:
    *   **Activation**: $A = \sum (InputBuffer_i \times Weights_i)$
    *   **Action**: $Output = Sign(Activation)$
    *   **Learning**: 
        *   If `Action == TargetPattern[t]`: Reward (+High).
        *   If `Action != TargetPattern[t]`: Punish (-Low).

### 3. Stability Monitor (Kernel Integration)
*   **Flux**: Measures change in Agent's Output Page.
*   **Lock Condition**: When `Flux < Threshold` for N ticks, the Agent is "Locked".

## Metrics
1.  **Lock Time**: Ticks until Agent output matches Pattern perfectly for >10 cycles.
2.  **Noise Tolerance**: Max % noise before Locking fails.
3.  **Energy Efficiency**: Does "Locking" reduce Flux/Energy consumption?

## Implementation Plan
1.  **Env**: Stream generator.
2.  **Agent**: Weighted accumulator + Hebbian update.
3.  **Main**: Loop measuring Lock Time vs Noise Level.
