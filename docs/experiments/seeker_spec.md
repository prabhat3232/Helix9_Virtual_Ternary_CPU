# Experiment 1: 1D Seeker (Cognitive Consensus Validation)

> **Status**: Locked
> **Date**: 2026-02-15

## 1. Goal
Validate that **Consensus** and **Decay** primitives alone (without weights, gradients, or probabilities) allow an agent to lock onto a signal in a noisy environment.

## 2. Environment Contract (Fixed)
The Environment (`seeker_env.cpp`) interacts with the Agent (`seeker.hasm`) via shared memory.

### Input Vector ($I_t$)
Memory Address: `0x3100`
Format: Sparse Ternary Vector (Length 256, but only index 0 used for 1D).
-   `I[0] = +1`: Goal is to the **Right**.
-   `I[0] = -1`: Goal is to the **Left**.
-   `I[0] =  0`: No Signal (Noise/Silence).

### Action Output ($A_t$)
Memory Address: `0x3200`
Format: Single Ternary Word.
-   `A = +1`: Move **Right**.
-   `A = -1`: Move **Left**.
-   `A =  0`: **Stay**.

### Reward Signal ($R_t$) (Implicit Context)
*Note: In this pure consensus model, "Reward" often shapes the Input $I$. If the agent moves closer, the signal $I$ gets stronger? Or simply $I$ remains the direction. For this experiment, $I$ **IS** the "Correct Direction". The Agent's job is to trust $I$ over time.*

**Noise Model**:
-   **10-30%** of cycles, Environment overwrites $I[0]$ with `0` (Signal Drop).
-   Agent must maintain belief during drops.

## 3. Agent Memory Layout (Fixed)
Restricted to Cognitive Memory Page 0 (`0x3000`).

| Region | Address | Size | Description |
| :--- | :--- | :--- | :--- |
| **Belief Manifold** | `0x3000` | 256 Words | The Agent's internal model. |
| **Input Buffer** | `0x3100` | 256 Words | Sensory Input (Write-Only by Env). |
| **Action Word** | `0x3200` | 1 Word | Motor Output (Read-Only by Env). |
| **Confidence** | `0x3300` | 1 Word | Calculated PopCount of Belief. |

## 4. Cognitive Rules (Deterministic)

### A. Learning (Consensus)
$$ S_{t+1} = \text{CNS}(S_t, I_t) $$
-   If $I_t = 0$ (Noise), $S$ does not change (Identity).
-   If $I_t \neq 0$ and $I_t == S_t$, $S$ reinforces.
-   If $I_t \neq 0$ and $I_t \neq S_t$, $S$ cancels to 0.

### B. Forgetting (Decay)
**Schedule**: Fixed every **4 Cycles**.
$$ S_{new} = \text{DEC}(S_{old}) $$
-   Eliminates stale beliefs. Prevents saturation from ruling forever.

### C. Decision (Action)
1.  **Calculate Confidence**: $C = \text{POP}(S_t)$.
2.  **Check Threshold**: Store in `0x3300`.
    -   If $C > \text{Threshold}$ (e.g., 5?): **ACT**.
    -   Else: **WAIT** (Action = 0).
3.  **Action Generation**:
    -   $A = \text{Sign}(\text{Sum}(S_t))$.
    -   (Since we only use Index 0, $A = S_t[0]$).

## 5. Metrics to Log
The Environment must log per timestep CSV:
1.  `Tick`: Cycle count.
2.  `Distance`: Agent distance to Goal.
3.  `Input`: $+1, -1, 0$.
4.  `Belief`: Value of $S[0]$.
5.  `Confidence`: PopCount value.
6.  `Action`: $+1, -1, 0$.
7.  `Sparsity`: % of Zero trits in Belief.

## 6. Expected Phases
1.  **Oscillation**: Belief fluctuates with noise.
2.  **Lock-in**: Belief stabilizes on correct direction.
3.  **Action**: Confidence breaches threshold, Agent moves.
4.  **Resilience**: Noise occurs, but Decay is slow enough that Belief survives gaps.

**Constraint**: NO Learning Rate. NO Gradients. NO Probabilities.
