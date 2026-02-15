# Helix Cognitive Runtime Specification (Phase 7C)
**Version:** 1.0 (Research-Grade)
**Status:** Frozen for Implementation

## 1. Overview
The **Cognitive Runtime Kernel** is the "OS layer" for Helix AI, transforming the Ternary CPU into a complete cognitive computing architecture. It manages the execution of cognitive agents, ensuring deterministic scheduling, stability monitoring, and autonomous reward processing. It introduces a **Cognitive Clock Domain** distinct from CPU cycles to scale AI workloads.

## 2. Core Architecture

### 2.1. Agent Abstraction
An `Agent` is the fundamental unit of cognition, managed by a strict state machine.

```cpp
struct Agent {
    uint32_t id;
    uint8_t  state;      // Lifecycle State
    uint8_t  priority;   // Scheduling Priority (0-255)
    uint64_t last_tick;  // Last execution cognitive tick
    
    // Memory Ownership (Page IDs)
    uint16_t belief_page_start;  // EXCLUSIVE (Read/Write)
    uint16_t belief_page_count;
    uint16_t input_page_start;   // READ-ONLY
    uint16_t output_page_start;  // WRITE-ONLY
    uint16_t shared_page_start;  // MEDIATED
    
    // Execution Context
    uint64_t pc;
    uint64_t sp;
    
    // Metrics
    int64_t health_score;
    int64_t stability_index;
    int64_t learning_rate;
};
```

### 2.2. Lifecycle State Machine
Explicit transitions define agent behavior.

| State | ID | Behavior | Transitions |
| :--- | :--- | :--- | :--- |
| **INIT** | 0 | Allocation, Memory Zeroing. | -> ACTIVE |
| **ACTIVE** | 1 | Normal "Sense-Act" loop. No Belief Update. | -> LEARNING, IDLE |
| **LEARNING** | 2 | "Sense-Update-Reward-Act". Plasticity Enabled. | -> CONVERGED, ACTIVE |
| **CONVERGED**| 3 | Stability achieved. Low-power/Maintenance mode. | -> ACTIVE (on Drift) |
| **IDLE** | 4 | Suspended (Paused). | -> ACTIVE, TERMINATED |
| **TERMINATED**| 5 | Cleaned up. Memory reclaimed. | None |

### 2.3. Memory Ownership Model
The runtime enforces isolation to prevent multi-agent corruption.
-   **Exclusive (Belief)**: Only owner Logic can R/W.
-   **Read-Only (Input)**: Sensors write, Agent reads. Trap on Agent Write.
-   **Write-Only (Output)**: Agent writes, Effectors read.
-   **Shared**: Lock-mediated interactions.

**Fault Containment**:
-   **Illegal Access**: Immediate trap -> Agent State = TERMINATED (or IDLE for debugging).
-   **Saturation**: If Energy > Threshold -> Reset Belief Page components.

## 3. The Scheduler (Kernel)

### 3.1. Cognitive Clock Domain
Usage of `cognitive_tick_rate` decouples CPU speed from Cognition.
-   **Tick**: One full pass of the Scheduler Loop.
-   **Ratio**: e.g., 1 Tick = 1000 CPU Cycles. Allows for slow belief updates vs fast I/O.

### 3.2. Fairness Policy
Deterministic but fair scheduling.
-   **Round-Robin** with **Weighted Priority**.
-   `max_cycles_per_agent_per_tick`: Hard limit on execution time to prevent monopolization.
-   **Starvation Prevention**: Aging boost for skipped agents.

### 3.3. Execution Loop
1.  **Sense**: Update Input Pages from Environment.
2.  **Execute**: Run Agent Quantum (`max_cycles`).
    *   Logic: `Belief = Consensus(Input, OldBelief)`.
    *   Decay: `Belief = Decay(Belief, Rate)`.
3.  **Stabilize**: Compute Metrics.
4.  **Reward**: Apply Reinforcement (If State == LEARNING).
5.  **Act**: Commit Output Pages.

## 4. Stability & Monitor

### 4.1. Metrics
-   **Flux** (Rate of Change):
    *   `Flux = PopCount(ConsensusDiff(B_t, B_{t-1}))`
    *   `ConsensusDiff(a, b)`: 0 if equal, 1 if changed. (Strict Ternary Diff).
-   **Energy** (Confidence): `PopCount(Abs(Belief))`.
-   **Coherence**: Multi-agent agreement.

### 4.2. Stability Triggers
-   **Oscillation**: High Flux + Low Net Change -> **Suspend** or **Reset**.
-   **Convergence**: Flux -> 0. -> Transition to **CONVERGED**.
-   **Divergence**: Energy Saturation. -> **Normalization** (Decay).

## 5. Reward Engine
Autonomous learning loop.

### 5.1. Normalization
Raw signals are dangerous.
`reward_norm = clamp(signal, -Rmax, Rmax)`
`reward_smoothed = (alpha * new) + ((1-alpha) * old)`

### 5.2. Mechanism
-   **Positive (>0)**: `VEC.CNS` with Reinforcement Pattern.
-   **Negative (<0)**: `DEC.MASK` (Decay) or Inversion.

## 6. Implementation Order
1.  **Agent Lifecycle**: State machine & Structs.
2.  **Scheduler Core**: Loop, Clock, Fairness.
3.  **Memory Isolation**: Permission logic in `TernaryMemory`.
4.  **Stability Monitor**: Flux/Energy calc corrections.
5.  **Reward Engine**: Normalization & Application.
6.  **Multi-Agent Test**: Concurrency & Noise tolerance.
