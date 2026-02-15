# Multi-Agent Swarm Experiment Specification

## Overview
A "Flocking" simulation where agents utilize **Shared Beliefs** to achieve consensus on movement direction. Unlike Seeker (Competition), this demonstrates **Cooperation** via the Cognitive Runtime's shared memory capabilities.

## Environment
*   **Grid**: 32x32 Toroidal (Reuse `S2DEnv`).
*   **No Food/Poison**: Pure movement dynamics.
*   **Agents**: 10-20 Agents spawned randomly.

## Agent Interface (Communication)
### 1. Input Page (Senses)
*   **Offsets 0-8**: Neighbors (Empty/Agent).
*   **Offsets 10-18**: **Neighbor Actions** (Read from neighbors' Output Pages).
    *   *Note*: In a real OS, this requires `ReadShared` permission or Kernel Copy.
    *   *Sim*: Kernel copies Neighbor Output -> Agent Input[10+].

### 2. Internal Logic (Consensus)
*   **Belief**: Desired Direction (Vector).
*   **Update Rule**:
    *   $Belief_{new} = CNS(Belief_{old}, NeighborActions)$
    *   If no neighbors: Random Walk / Keep Inertia.
    *   If neighbors: Align with average direction.
*   **Stability**: Flux should decrease as swarm aligns.

## Metrics
*   **Cohesion**: Average distance to nearest neighbor (minimize).
*   **Alignment**: Variance of Agent Directions (minimize to 0).
*   **Convergence Time**: Ticks to reach steady state (Alignment < Threshold).

## Implementation Plan
1.  **`swarm_env.cpp`**: Simplified `S2DEnv` (No items).
2.  **`swarm_agent.cpp`**:
    *   `Sense()`: Reads neighbors AND their previous actions.
    *   `Decide()`: Implements Consensus Logic.
3.  **`swarm_main.cpp`**: Runs loop and calculates Alignment metric.
