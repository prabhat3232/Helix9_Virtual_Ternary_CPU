# Helix-9: Virtual Ternary CPU & Sparse AI Engine

> **Research Paper**: [read the full paper here](docs/research_paper.md).

**Helix-9** is an experimental Virtual Processor architecture exploring **Balanced Ternary** logic (-1, 0, 1) for native sparse AI training. Unlike binary systems where `0` is a value, Helix-9 architecture treats `0` as a "skip" signal, enabling O(N) compute scaling with active connections.

## Key Research Findings (2026)

Our emulation engine demonstrates that:
1.  **Ternary Backprop Works**: A custom TernaryFloat format successfully trains neural networks (Loss: 25.0 vs Start: 2500).
2.  **Sparsity = Speed**: Even in software emulation, sparse models (50% density) run **1.18x faster** than dense ones.
3.  **Capacity Value**: We identified "Capacity Collapse" in small sparse networks, confirming the importance of network scale.

## Core Features
- **Virtual CPU**: 16-register Ternary ISA.
- **Sparse Engine**: Structure-of-Arrays (SoA) accelerator for sparse vectors.
- **C++ Fit Loop**: Optimized training loop with 1.1x speedup over Python.
- **LUT Arithmetic**: Integer Look-Up Tables replacing expensive powers-of-3 logic.

## Quick Start

```cmd
build.bat
python benchmark.py
```

## Project Structure

This repository contains the C++ Core Engine and Python Bindings.

```
Helix9_Virtual_Ternary_CPU/
├── src/
│   ├── cpu.cpp          # The Virtual CPU (Registers, Opcode Decoder)
│   ├── memory.cpp       # Paged Ternary Memory System
│   ├── neural_net.h     # Sparse AI Engine (Parallel Vectors, Topology-Aware Sparsity)
│   ├── sparse_math.h    # Sparse Vector/Matrix Implementations (SoA)
│   ├── soft_float.cpp   # Optimized TernaryFloat Arithmetic (Lookup Tables)
│   ├── bindings.cpp     # C-API for Python Integration (Fit Function)
│   └── ...
├── helix9.py            # Python Wrapper (ctypes)
├── benchmark.py         # Comprehensive Performance Benchmark
├── CMakeLists.txt       # Main Build Configure
└── README.md            # This file
```

## 1. The C++ Core Engine

The engine is written in C++17 for performance. It simulates:
- **CPU**: 16 Registers (R0-R15), PC, SP.
- **ALU**: Ternary Addition, Multiplication, Min/Max logic.
- **AI Co-Processor**: A hardware-accelerated Sparse Neural Network implementation using **Parallel Vectors** for cache efficiency.

### Key Improvements
- **C++ Fit Loop**: Moves the entire training loop to C++ to eliminate Python overhead (1.1x speedup).
- **Lookup Tables**: Replaces expensive power calculations with O(1) array lookups (15% speedup).
- **Topology-Aware Sparsity**: Guarantees valid connections for every input neuron during sparse initialization, preventing "dead inputs".

### Building & Running
Requirements: CMake 3.10+ and MSVC/MinGW.

```cmd
build.bat
```

## Tested Environment
This project has been verified on the following hardware and software configuration:
- **OS**: Microsoft Windows 11 Pro (Build 26200)
- **CPU**: Intel Core i3-9100F @ 3.60GHz
- **RAM**: 24 GB
- **Compiler**: MSVC (via Visual Studio Build Tools)
- **CMake**: 3.31.2
- **Python**: 3.10.0


## 2. Python Integration

Helix-9 provides a Python wrapper (`helix9.py`) that interfaces with the compiled C++ DLL (`helix9_lib.dll`).

### Setup
1.  Run `build.bat` to compile `helix9_lib.dll`.
2.  Ensure `helix9.py` is in your working directory.

### High-Performance Training (Recommended)
Use `fit()` to run the training loop entirely in C++:

```python
from helix9 import Helix9

helix = Helix9()
helix.create_ai()
helix.add_layer(15, 40, density=0.8) # 80% Density (20% Sparsity)
helix.add_layer(40, 3, density=1.0) 

# Train 5000 epochs in C++ (Fastest)
loss = helix.fit(inputs, targets, epochs=5000)
print(f"Final Loss: {loss}")
```

### Manual Loop (Advanced)
For custom training logic, use `train_step()` and `apply_updates()`:

```python
for epoch in range(100):
   # Accumulate gradients (Batch)
   for x, y in zip(inputs, targets):
       helix.train_step(x, y)
   
   # Apply updates once per batch
   helix.apply_updates()
```

## 3. Benchmarks

Run `benchmark.py` to compare Dense vs Sparse performance.

| Model | Density | Time (10k epochs) | Result |
|-------|---------|-------------------|--------|
| **Dense** | 100% | 259s | ✅ Converged |
| **Sparse** | 50% | 219s | ⚡ Fastest |

## Key Concepts

### Balanced Ternary
- **Values**: -1 (False/Negative), 0 (Unknown/Neutral), +1 (True/Positive).
- **Math**: Addition doesn't need a sign bit. `(+1) + (-1) = 0`.

### Sparse Parallel Vectors
Modern GPUs optimize for dense matrix multiplication. Helix-9 optimizes for **Sparsity**.
-   **Structure**: Instead of HashMaps or Dense Matrices, we use parallel vectors (`Indices`, `Values`) for gradients and weights.
-   **Momentum**: The optimizer logic is designed to decay momentum correctly even on zero-gradient connections, ensuring physical stability during sparse training.

## License
This project is licensed under the **Apache 2.0 License**. See the `LICENSE` file for details.

## Citation
If you use Helix-9 architecture in your research, please cite:

```bibtex
@article{helix9_2026,
  title={Helix-9: High-Performance Sparse Training on a Virtual Ternary Processor},
  author={Prabhat},
  year={2026},
  url={https://github.com/prabhat3232/Helix9_Virtual_Ternary_CPU}
}
```

## Disclaimer
**Experimental Research Software**: This project is a Proof-of-Concept (PoC) architecture. It is designed for scientific validation of ternary sparse training and is not intended for production environments.

## Acknowledgements
**Concept & Research**: Prabhat  
**Implementation**: Executed via **Vibe Coding** (AI-Assisted Development).

---
*Created by Prabhat (2026).*


