# Helix9: Virtual Ternary CPU & Cognitive Architecture
**Status**: Research Prototype v1.0 (Phase 8 Complete)
**License**: MIT

---

## 1. Overview
**Helix9** is an experimental Virtual Machine architecture designed to explore the advantages of **Balanced Ternary Logic** (`-1, 0, +1`) in Artificial Intelligence and Cognitive Emulation.

Unlike traditional binary systems that force "True/False" dichotomies, Helix9 natively represents "Positive", "Negative", and "Unknown" states. This ternary foundation allows for highly efficient modeling of **Consensus**, **Decay**, and **Weighted Belief Systems** directly in hardware, reducing the computational complexity of large-scale agent simulations.

### Key Innovations
*   **27-Trit Architecture**: A native word size of 27 trits (approx 42-43 bits), providing high precision for fixed-point math and neural weights.
*   **Cognitive Primitives**: Hardware instructions for `CNS` (Consensus), `DEC` (Decay), and `POP` (Ternary PopCount).
*   **Vector Acceleration**: A dedicated Vector Unit (SIMD) for cognitive operations, achieving **10x speedups** over scalar emulation.
*   **Cognitive Runtime Kernel**: A lightweight OS layer optimized for massive multi-agent scheduling and memory management.

*   **Cognitive Runtime Kernel**: A lightweight OS layer optimized for massive multi-agent scheduling and memory management.

### Real-World Applications
While Helix9 is a research architecture, its ternary foundation offers concrete advantages for specific domains:
1.  **Ultra-Low Power Edge AI**: Ternary weights (`-1, 0, 1`) allow neural networks to use **sparsity** (skipping 0s) naturally. This eliminates 60-80% of multiplications, making it ideal for battery-powered IoT devices running inference.
2.  **Robust Control Systems**: Balanced ternary logic is inherently smoother for control loops (PID, Robotics) as it avoids the "offset bias" of unsigned binary numbers.
3.  **Error-Resilient Computing**: Ternary logic has higher information density per wire, potentially reducing circuit complexity and improving reliability in high-radiation environments (e.g., Space Systems).
4.  **Swarm Robotics**: The hardware `Consensus` primitives enable efficient agreement protocols for hundreds of autonomous agents (drones) with minimal code.

---

## 2. Architecture
### 2.1 The Ternary ISA
The Instruction Set Architecture is designed around 42-bit (27-trit) instructions.
*   **Arithmetic**: `ADD`, `SUB`, `MUL`, `DIV`, `MOD` (Signed Balanced Ternary - No Two's Complement required).
*   **Logic**: `AND` (Min), `OR` (Max), `XOR` (Sum Modulo 3).
*   **Cognitive**:
    *   `CNS Rd, Rs1, Rs2`: Calculates the consensus of two values (averaging with bias).
    *   `DEC Rd, Rs1`: Decays a value towards 0 (mimicking neural potential decay).
    *   `POP Rd, Rs1`: Counts the total "energy" (non-zero trits) in a word/vector.
*   **Vector**:
    *   `VEC.CNS`: Performs element-wise consensus on 256-word pages in 1 cycle (hardware model).

### 2.2 Memory Model
*   **Sparse Cognitive Pages**: Memory is organized into 256-word pages. Pages are allocated on-demand, allowing sparse agent states (90% memory savings for inactive agents).
*   **System Memory**: First 12KB (Pages 0-11) reserved for Kernel/OS.
*   **Protection**: Hardware-enforced Read/Write permissions based on Agent ID (Context).

---

## 3. Toolchain
The Helix9 ecosystem includes a complete toolchain for developing and running ternary software.

### 3.1 Assembler (`helix_asm`)
Compiles `.hasm` assembly files into `.ht` (Helix Ternary) object files.
*   **Features**: Label resolution, macro support, `.WORD` / `.ORG` directives.
*   **Usage**: `helix_asm input.hasm -o output.ht`

### 3.2 Linker (`helix_ld`)
Combines multiple `.ht` object files into a single executable, resolving external symbols and relocating addresses.
*   **Usage**: `helix_ld main.ht lib.ht -o app.ht`

### 3.3 Emulator (`helix_emu`)
A cycle-accurate simulator of the Helix CPU.
*   **Features**: Memory protection simulation, debug tracing, register dumps.
*   **Usage**: `helix_emu app.ht`

### 3.4 Benchmarks (`helix_bench`)
A harness for verifying performance metrics.
*   **Capabilities**: Cycle counting, MIPS calculation, Vector Latency modeling.

---

## 4. The Cognitive Runtime Kernel
The **Kernel** serves as the Operating System for the Helix architecture, managing the lifecycle of thousands of autonomous agents.

*   **Scheduler**: Implements "Cognitive Time-Slicing". Agents are scheduled based on their **Flux** (rate of state change)—active agents get more CPU time, stable agents sleep.
*   **Memory Manager**: Handles page faults and allocation for agent beliefs/vectors.
*   **Signal Bus**: A publish-subscribe system for inter-agent communication (simulating pheromones or broadcast signals).

---

## 5. Experimental Results
We validated the architecture through a series of "Cognitive Experiments".

### Exp 1: Survival (2D Seeker)
*   **Goal**: Agents must navigate a 2D grid to find food sources while their energy decays.
*   **Result**: Agents successfully used Ternary Vectors (`+1`=Right, `-1`=Left) to navigate.
*   **Insight**: Ternary logic simplified the "Movement Vector" calculation significantly compared to binary checks.

### Exp 2: Swarm Consensus
*   **Goal**: A swarm of 100 agents must agree on a travel direction using only local interactions.
*   **Result**: Achieved **80% Directional Alignment** using the `CNS` instruction.
*   **Insight**: The hardware `CNS` primitive allowed flocking behavior to emerge with zero conditional logic (if/else).

### Exp 3: Cognitive Resonance (Pattern Locking)
*   **Goal**: An agent must "lock on" to a specific signal pattern (`+1, -1, +1`) amidst 30% background noise.
*   **Result**: **95.5% Accuracy** in signal detection.
*   **Insight**: The `POP` (PopCount) instruction provided a robust metric for "Confidence" in the locked signal.

---

## 6. Performance Benchmarks
(Updated: 2026-02-16)

Benchmarks verify the efficiency of the Cognitive Primitives.

| Benchmark | Cycles | Notes |
| :--- | :--- | :--- |
| **Base Arithmetic** | 4006 | Baseline scalar performance. |
| **Cognitive Ops** | 6006 | Scalar Implementation of `CNS`/`DEC`. |
| **Agent Cycle** | 5908 | Full Sense-Act loop (100 iterations). |
| **Vector Soft** | 2567 | Scalar loop processing 256 elements. |
| **Vector Hard** | 260 | Hardware Vector Unit (Simulated Latency). |

**Conclusion**: The Vector Unit delivers a verified **~10x Speedup** for cognitive workloads.

---

## 7. Build & Usage
### Prerequisites
*   CMake 3.10+
*   C++17 Compiler (MSVC / GCC / Clang)

### Build
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```

### Running the Emulator
```bash
./build/Debug/helix_emu tests/test_alu.exe
```

### Running Experiments
```bash
./build/Debug/seeker_2d      # Run Exp 1
./build/Debug/swarm_exp      # Run Exp 2
./build/Debug/resonance_exp  # Run Exp 3
```

---

## 7. Verification Suite
The system is validated by a comprehensive suite of unit and integration tests.

### Unit Tests
*   `test_alu`: Verifies specific arithmetic and logic operations (ADD, MUL, CNS, etc.).
*   `test_cpu`: Validates instruction decoding, flag updates, and register behavior.
*   `test_memory`: Checks page allocation, permissions, and cognitive page logic.
*   `test_asm_exec`: Runs small assembly snippets to verify assembler-emulator integration.

### Integration Tests
*   `test_cognitive_runtime`: Simulates the Kernel Scheduler and multi-agent context switching.
*   `test_ai`: Verifies high-level AI constructs (Flux, Entropy, Reward) in isolation.
*   `test_trace`: Ensures the debugging trace system correctly captures CPU states.

To run the full suite:
```bash
ctest -C Debug --output-on-failure
```

---

## 8. Future Roadmap
*   **FPGA Implementation**: Synthesize the core to physical hardware (Verilog/VHDL).
*   **Advanced Compiler**: Port a subset of C/Rust to Helix Assembly.
*   **Neural Network Layer**: Implement a complete Ternary Neural Network (TNN) using the Vector Unit.

## 9. Acknowledgments
Developed by **Prabhat**.
Copyright 2026 Prabhat.

Designed for **Advanced Agentic Coding** research.

