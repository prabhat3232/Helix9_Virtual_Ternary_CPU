# Helix Python Language Specification (v0.1)

## 1. Overview
Helix Python is a restricted subset of Python 3 that compiles directly to Helix9 Assembly (`.hasm`). It is designed for high-performance AI kernels and system logic.

## 2. Compilation Model
- **Source**: Standard `.py` files using the `ast` module.
- **Entry Point**: Functions decorated with `@helix.jit`.
- **Output**: Pure Helix9 Assembly code.

## 3. Data Types

### 3.1 Scalar Integers (`int`)
- Maps to: `TernaryWord` (27-trit signed).
- Range: $\pm 3.8 \times 10^{12}$.
- Storage: CPU Registers or Stack.

### 3.2 Matrices (`Matrix[Rows, Cols]`)
- Maps to: Contiguous blocks in `.DAT` (RAM).
- **Declaration**: `x: Matrix[16, 16]` (Type hint required for allocation).
- **Semantics**: Value semantics (compiler manages pointers).

## 4. Supported Syntax

### 4.1 Control Flow
```python
if a > b:
    ...
else:
    ...

while x < 10:
    ...

# For loops are unrolled or mapped to while
for i in range(10):
    ...
```

### 4.2 Functions
```python
@helix.jit
def my_kernel(a: int, b: int) -> int:
    return a + b
```

### 4.3 Operators
- **Arithmetic**: `+`, `-`, `*`, `/`, `%`
- **Ternary Logic**: `&` (Min), `|` (Max), `^` (SumMod3)
- **Comparison**: `==`, `!=`, `<`, `>`, `<=`, `>=`
- **Matrix**: `@` (MatMul), `+` (Element-wise Add), `-` (Element-wise Sub)

## 5. Matrix Operations (AI Accelerator)

The compiler recognizes Matrix types and emits specialized Vector Instructions.

### 5.1 Matrix Multiplication
```python
C = A @ B
```
**Compiles to**:
- Loops over Rows/Cols.
- Uses `VEC.SAT.MAC` for hardware acceleration.
- Automatic memory management for result `C`.

### 5.2 Element-wise Ops
```python
C = A + B
D = A - B
```
**Compiles to**:
- `VEC.ADD` / `VEC.SUB` loops.

## 6. Memory Management
- **Globals**: Allocated in `.DAT` section (static addresses).
- **Locals**: Allocated on Stack (`SP`).
- **Matrices**: Allocated in `.DAT` (Compiler-managed heap/static area) to simplify base pointer usage.

## 7. Example Kernel

```python
from helix_lang import Matrix, helix_jit

@helix_jit
def dense_layer(input: Matrix[1, 256], weights: Matrix[256, 10], bias: Matrix[1, 10]) -> Matrix[1, 10]:
    # Efficiently compiles to ~20 instructions
    return (input @ weights) + bias
```
