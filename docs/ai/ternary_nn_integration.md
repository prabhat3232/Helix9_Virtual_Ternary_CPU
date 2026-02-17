# Helix9: Ternary Neural Network (TNN) Integration Plan

> **Goal**: Train Ternary Neural Networks in Python (PyTorch) and deploy them to Helix9 for inference.

## 1. Overview
This workflow bridges modern Deep Learning frameworks with the Helix9 Ternary Architecture. By constraining weights and activations to `{-1, 0, +1}` during training, we can export models that run natively on Helix9's `SAT.MAC` (Saturating Multiply-Accumulate) hardware.

## 2. Architecture: The "Bridge"
We do not run Python on Helix9. Instead, we use Python as a **Compiler** that produces HelixASM data.

```mermaid
graph LR
    A[PyTorch Model] -->|Quantization Aware Training| B[Ternary Weights]
    B -->|Export Script| C[HelixASM Data (.DAT)]
    C -->|Assembler| D[Helix Executable (.htx)]
    D -->|Inference| E[Helix9 CPU]
```

## 3. Python Side (Training)
**Location**: `experiments/python/`

### 3.1 Custom Layers (`ternary_layers.py`)
We must define PyTorch layers that simulate Helix9 hardware:
-   **Forward Pass**: Round weights/inputs to `{-1, 0, 1}`.
-   **Backward Pass**: Use "Straight-Through Estimator" (STE) to allow gradients to flow through the rounding function.
-   **Activation**: `HardTanh` or custom step function (no ReLU/Sigmoid).

### 3.2 Model Architecture (`mnist_ternary.py`)
A simple Multi-Layer Perceptron (MLP) for MNIST:
-   Input: 784 ($28 \times 28$) -> Trinarized to `{-1, +1}`.
-   Hidden: 128 Neurons (Ternary Weights).
-   Output: 10 Classes (Accumulators).

### 3.3 Export Logic (`export_helix.py`)
Converts trained weights into HelixASM format:
-   **Weights**: `.DAT` blocks of ternary values.
-   **Layout**: Column-major or Row-major to match `VEC` instruction memory access patterns.

## 4. Helix9 Side (Inference)
**Location**: `experiments/tnn/`

### 4.1 Inference Kernel (`inference.hasm`)
A tight loop utilizing the Vector Unit:
1.  **Load Input**: Read test image into Memory Page 0.
2.  **Layer 1**: 
    -   Iterate through neurons.
    -   `VEC.SAT.MAC` (Vector Saturating MAC) to compute dot product of Input Page vs Weight Page.
    -   Apply Threshold (Activation).
    -   Store to Hidden Layer Page.
3.  **Layer 2**: Repeat for Output.
4.  **ArgMax**: Scan output page for highest value.

## 5. Implementation Roadmap

### Phase 1: Python Foundation
- [ ] Create `experiments/python/ternary_ops.py` (Quantization logic).
- [ ] Implement `TernaryLinear` layer in PyTorch.
- [ ] Train trivial "XOR" gate to verify convergence.

### Phase 2: MNIST Training
- [ ] Train 784-128-10 network on MNIST.
- [ ] achieve > 90% accuracy with ternary constraints.
- [ ] Implement `export_weights.py` to generate `weights.hasm`.

### Phase 3: Helix Integration
- [ ] Create `experiments/tnn/mnist_inference.hasm`.
- [ ] Implement `LoadImage` routine (mock input).
- [ ] Execute `VEC.SAT.MAC` loop.
- [ ] Verify Helix9 prediction matches PyTorch prediction.
