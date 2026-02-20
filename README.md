# Helix9: Balanced Ternary AI Accelerator & VM

#**Status**: Research Prototype v1.0 (Phase 11 Complete)
#**License**: MIT
#**Methodology**: 100% Vibe Coding (AI-Agent Driven Development)

---

## 1. Overview
**Helix9** is an experimental Virtual Machine architecture designed to evaluate the computational trade-offs of **Balanced Ternary Logic** (`-1, 0, +1`) for sparse neural and multi-agent workloads.

Unlike traditional binary systems that force "True/False" dichotomies, Helix9 natively represents "Positive", "Negative", and "Unknown" states. This ternary foundation allows for highly efficient modeling of **Consensus**, **Decay**, and **Weighted Belief Systems** directly in hardware, reducing the computational complexity of large-scale agent simulations.

### Key Innovations
*   **27-Trit Architecture**: A native word size of 27 trits (approx 42-43 bits), providing high precision for fixed-point math and neural weights.
*   **Cognitive Primitives**: Hardware instructions for `CNS` (Consensus), `DEC` (Decay), and `POP` (Ternary PopCount).
*   **Vector Acceleration**: A dedicated Vector Unit (SIMD) for cognitive operations, achieving **10x speedups** over scalar emulation.
*   **Cognitive Runtime Kernel**: A lightweight OS layer optimized for massive multi-agent scheduling and memory management.
*   **TNN Graph Compiler**: Directly compiles abstract neural network descriptions (`.htnn`) into native, fused Helix9 assembly executed directly on the Vector Unit.

### Real-World Applications
While Helix9 is a research architecture, its ternary foundation offers concrete advantages for specific domains:
1.  **Ultra-Low Power Edge AI**: Ternary weights (`-1, 0, 1`) allow neural networks to use **sparsity** (skipping 0s) naturally. This eliminates 60-80% of multiplications, making it ideal for battery-powered IoT devices running inference.
2.  **Robust Control Systems**: Balanced ternary logic is inherently smoother for control loops (PID, Robotics) as it avoids the "offset bias" of unsigned binary numbers.
3.  **Error-Resilient Computing**: Ternary logic has higher information density per wire, potentially reducing circuit complexity and improving reliability in high-radiation environments (e.g., Space Systems).
4.  **Swarm Robotics**: The hardware `Consensus` primitives enable efficient agreement protocols for hundreds of autonomous agents (drones) with minimal code.

---

## 2. Architecture

```text
+-----------------------------------------------------------+
|                    TNN Graph Compiler                     |
|  .htnn Models -> Fused IR -> Opcode Mapping (0x4000)      |
+-----------------------------------------------------------+
                            |  Instruction Stream
+-----------------------------------------------------------+
|           Helix9 Virtual CPU (27-Trit Architecture)       |
|                                                           |
|  +---------------+  +---------------+  +---------------+  |
|  |  Scalar Core  |  |  Vector Unit  |  | Cognitive ALU |  |
|  |  Registers 16 |  |  V0-V3 (1024) |  |   CNS, DEC    |  |
|  +---------------+  +---------------+  +---------------+  |
+-----------------------------------------------------------+
                            |  12KB Bus
+-----------------------------------------------------------+
|              Sparse Ternary Memory (Pages)                |
|  [0x0000: OS/Kernel]  [0x2000: Tensor Heap]               |
+-----------------------------------------------------------+
```

### 2.1 The Ternary ISA
The Instruction Set Architecture is designed around 42-bit (27-trit) instructions.

**Encoding Format:**
```text
| OPCODE (6 Trits) | MODE (3 Trits) | RD (4 Trits) | RS1 (4 Trits) | RS2 / IMM (10 Trits) |
```

*   **Arithmetic**: `ADD`, `SUB`, `MUL`, `DIV`, `MOD` (Signed Balanced Ternary - No Two's Complement required).
*   **Logic**: `AND` (Min), `OR` (Max), `XOR` (Sum Modulo 3).
*   **Cognitive**:
    *   `CNS Rd, Rs1, Rs2`: Calculates the consensus of two values (averaging with bias).
    *   `DEC Rd, Rs1`: Decays a value towards 0 (mimicking neural potential decay).
    *   `POP Rd, Rs1`: Counts the total "energy" (non-zero trits) in a word/vector.
*   **Vector**:
    *   `VEC.CNS`: Performs element-wise consensus on 256-word pages in 1 cycle (hardware model).
    *   `VLDR`/`VSTR`: Vector Load/Store (Length=32).
    *   `VADD`: Vector Addition.
    *   `VDOT`: Vector Dot Product (TNN Acceleration).
    *   `VMMUL`/`VMMSGN`: Vector-Matrix Multiplication (with explicit Fused Ternary Sign Activation bypassing memory).

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

### 3.5 Python Compiler (`helix_py_compiler`)
Compiles a subset of Python 3 into Helix Assembly (`.hasm`).
*   **Features**:
    *   Variables, Integers, loops (`while`), conditionals (`if`).
    *   **Matrix Support**: Native 2D Matrix declaration (`Matrix[R,C]`) and Element Access (`A[i,j]`).
    *   **Output**: standard `print()` support.
*   **Usage**: `python src/compiler/py/helix_py_compiler.py input.py`

### 3.6 TNN Graph Compiler (`HelixRuntime`)
A native execution orchestrator that loads `.htnn` models, applies Operator Fusion (`Dense` + `Sign` -> `VMMSGN`), statically allocates non-overlapping pages in the Ternary Virtual Memory, and emits native Helix9 opcode structs directly into executable memory, eliminating assembler overhead and C++ layer simulation.


---

## 4. The Cognitive Runtime Kernel
The **Kernel** serves as a simulation runtime for experiments rather than a bare-metal OS (hardware-level traps and context restoration are simulated). It manages the lifecycle of thousands of autonomous agents.

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

### Appendix A: Methodology for Reproducibility
*   **Swarm Density**: 100 autonomous agents.
*   **Environment Grid**: 64x64 discrete ternary map layout.
*   **Simulation Length**: Verified over 100-3000 iterative sense-act cycles.
*   **Noise Model**: 30% uniform random bit-flip distribution on input percepts to test `Consensus` resilience.

---

## 6. Performance Benchmarks
(Updated: 2026-02-19)

Benchmarks verify the efficiency of the Cognitive Primitives.

### Group 1: Scalar Baseline
| Benchmark | Cycles | Notes |
| :--- | :--- | :--- |
| **Base Arithmetic** | 4006 | Baseline scalar performance. |
| **Cognitive Ops** | 6006 | Scalar Implementation of `CNS`/`DEC`. |
| **Scalar Dot (N=32)** | 453 | Baseline Loop for TNN. |
| **Agent Cycle** | 5908 | Full Sense-Act loop (100 iterations). |

### Group 2: Vector Acceleration
| Benchmark | Cycles | Notes |
| :--- | :--- | :--- |
| **Vector Soft (N=256)** | 2567 | Scalar loop processing 256 elements. |
| **Vector Hard (N=256)** | 260 | Hardware `VEC.CNS` (**~10x Speedup**). |
| **Vector Dot (N=32)** | 98 | Hardware `VDOT` (**4.6x Speedup**). |
| **Matrix Traditional (N=32x32)** | 1090 | `VMMUL` followed by `VSIGN`. |
| **Matrix Fused (N=32x32)** | 1058 | Fused `VMMSGN` (**3% Absolute Speedup**)* |

*\*Note on Fusion: The 3% `VMMSGN` fusion gain at N=32x32 reflects that memory bandwidth (loading ternary weights) dominates the cycle-time bounds compared to the negligible arithmetic cost of the Sign function. However, the fusion entirely bypasses intermediate memory store-load roundtrips, critical for strict zero-copy inference.*

**Conclusion**: The hardware Vector Unit delivers a verified **4.6x - 10x Speedup** for cognitive and neural workloads over scalar baselines.

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
./build/bin/Debug/helix_emu tests/test_alu.exe
```

### Running Experiments
```bash
./build/bin/Debug/seeker_2d      # Run Exp 1
./build/bin/Debug/swarm_exp      # Run Exp 2
./build/bin/Debug/resonance_exp  # Run Exp 3
./build/bin/Debug/tnn_benchmark  # Run TNN Vector Benchmark
```

### Running Python Scripts
Use the automated pipeline script to Compile, Assemble, and Emulate Python code in one step:
```bash
python run_helix.py tests/test_matrix_access.py
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
*   **Advanced Compiler**: Complete Python support (MNIST Inference) and explore C/Rust subsets.
*   **Dynamic Tensor Batching**: Expand the Graph Compiler `VSTRIDE` utilization to allow multi-batch TNN inferences.

## 9. Acknowledgments
Developed by **Prabhat**.
Copyright 2026 Prabhat.

Designed for **Advanced Agentic Coding** research and built entirely via **Vibe Coding**.

