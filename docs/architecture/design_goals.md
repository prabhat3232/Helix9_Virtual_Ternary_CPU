# Helix9 Design Goals & Research Philosophy
**Phase-1 Architecture**
**Version:** v0.1

---

## 1. Research Mission
The primary goal of Helix9 is to **prove where Ternary Computing outperforms Binary**.
We are not building a general-purpose replacement for x86/ARM. We are building a specialized architecture to answer specific research questions about:
1.  **Sparsity**: Does `0` as a hardware state enable O(N) scaling for sparse AI?
2.  **Density**: Can balanced ternary reduce instruction counts for complex math?
3.  **Control Flow**: Does 3-way branching reduce pipeline stalls?

## 2. Core Philosophy

### 2.1 Minimalism
*   **Principle**: If a feature does not directly support the research mission, it is excluded.
*   **Practice**: No complex addressing modes, no legacy support, no variable-length instructions. The ISA is orthogonal and RISC-like.

### 2.2 Compiler-First
*   **Principle**: The architecture must be a valid target for LLVM.
*   **Practice**:
    *   Regular register file (no accumulator-based bottlenecks).
    *   Standard calling conventions.
    *   No "magic" side-effects in instructions.
    *   Uniform instruction format to simplify code generation.

### 2.3 Mathematical Consistency
*   **Principle**: Balanced Ternary is the source of truth.
*   **Practice**:
    *   No "Signed vs Unsigned" integer types. All numbers are signed by nature.
    *   Arithmetic wraps around naturally.
    *   Logic gates are defined by ternary truth tables, not binary emulations.

### 2.4 AI-Native
*   **Principle**: The hardware should accelerate Neural Network primitives.
*   **Practice**:
    *   Native support for fused multiply-add (eventually).
    *   Efficient handling of `0` values (skipping storage/computation).
    *   Vector-friendly opcode reservation.

---

## 3. Success Metrics for Phase-1
Phase-1 is considered successful when:
1.  **Specification is Unambiguous**: An assembler can be written without asking the architect questions.
2.  **Turing Complete**: The ISA can implement any standard algorithm (Sorting, FFT, Matrix Mul).
3.  **Determinism**: The virtual CPU executes identically on any host machine.

---

## 4. Target Workloads
*   **Sparse Neural Inference**: Running MLP/RNN layers with >80% sparsity.
*   **Probabilistic Computing**: Algorithms utilizing the "Unknown" (`0`) state.
*   **Signal Processing**: FFTs where ternary coefficients map to sine waves naturally.
