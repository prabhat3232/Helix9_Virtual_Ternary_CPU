# Helix Vector ISA & Cognitive Runtime Specification (v0.1)

> **Status**: Experimental (Phase 7)
> **Tier**: Core (Permanent Extension)

## 1. Vector Execution Model (VEC)

### 1.1 Architecture
-   **Vector Length**: 256 Words (1 Memory Page).
-   **Atomicity**: All `VEC` operations are atomic at the page level.
-   **Interrupts**: Deferred until the instruction completes the full 256-word sweep.
-   **Hardware Scan Order**: Strict Deterministic `Word 0` -> `Word 255`.

### 1.2 Optimization
-   **Zero-Page Skip**: If a Source Page is marked `NULL` (Sparse Memory), the operation MAY exit early if the logic allows (e.g., Consensus with Null = Null).
-   **Sparse Write**: If a `VEC` operation results in a Zero Page, the Hardware (or Emulator) MAY deallocate the page and mark it `NULL`.

## 2. Vector ISA (`VEC`)

| Opcode | Operands | Description | Cognitive Role |
| :--- | :--- | :--- | :--- |
| `VEC.CNS` | `Pd, Ps1, Ps2` | Page Consensus. $P_d[i] = CNS(P_{s1}[i], P_{s2}[i])$ | Belief Update |
| `VEC.POP` | `Rd, Ps1` | Population Count. $R_d = \sum_{i=0}^{255} |P_{s1}[i]|$ | Confidence Measure |
| `DEC.MASK`| `Pd, Ps1, Ps2` | Masked Decay. $P_d[i] = DEC(P_{s1}[i], P_{s2}[i])$ | Forgetting / Noise Filter |
| `SAT.MAC` | `Rd, Ps1, Ps2` | Saturating MAC. $R_d = \sum SAT(P_{s1}[i] \times P_{s2}[i])$ | Weighted Reward / Value |

### 2.1 Opcode Details
-   **VEC.CNS**: Uses "Learning Consensus" logic: $(A \& \sim -B) | (B \& \sim -A)$.
-   **DEC.MASK**: Uses Mask Logic (`Ps2`). If $Ps2[i]$ is 0, $P_{s1}[i]$ is cleared.
-   **SAT.MAC**: Accumulates into a scalar register `Rd`. Saturates at Max/Min integer.

## 3. Cognitive Scheduler
The Runtime coordinates cognition via a strict loop ("The Cognitive Cycle"):

1.  **Sense**: Load Input Buffer from IO/Sensors.
2.  **Update**: `VEC.CNS` (Integrate Input into Belief).
3.  **Stabilize**: `VEC.POP` (Measure new Confidence).
    -   Check **Convergence** ($\Delta Conf < \epsilon$).
    -   Check **Oscillation** (History Buffer).
4.  **Reward**: `SAT.MAC` (Apply feedback/weights if RL enabled).
5.  **Decide**: Compare Confidence vs Threshold -> `ACT` or `WAIT`.
6.  **Orchestrate**: Switch Context (if Multi-Agent).

## 4. Stability Mathematics
Formal definitions for System Stability:

-   **Convergence**: When $\frac{d}{dt} Conf(S) \approx 0$.
    -   Implementation: $|\text{POP}(S_t) - \text{POP}(S_{t-1})| \le \epsilon$.
-   **Attractor**: A state $S$ such that $CNS(S, I) \to S$ for a set of Inputs $\{I\}$.
-   **Collapse**: When Sparsity $> 99\%$ (Belief Death). Trigger `Reset`.
-   **Oscillation**: When $S_t \approx S_{t-k}$ for small $k$.

## 5. Debug & Observability
Required instrumentation for "Glass Box" AI:

-   **Belief Dump**: Binary/CSV export of full 256-word pages.
-   **Resonance Log**: Track when $CNS$ outcomes exceed magnitude threshold.
-   **Snapshotting**: Periodic RLE-compressed dumps of the entire Belief Manifold.
