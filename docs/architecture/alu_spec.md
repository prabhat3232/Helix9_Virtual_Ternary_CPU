# ALU Specification
**Phase-1 Architecture**
**Version:** v0.1

---

## 1. Arithmetic Operations

### 1.1 Addition (`ADD` / `SUB`)
*   **Input**: Two 27-trit words.
*   **Output**: One 27-trit result.
*   **Flags Updated**:
    *   `Z`: Set if result is 0.
    *   `P`: Set if result > 0.
    *   `N`: Set if result < 0.
    *   `C`: Set if carry generated out of MSB.
    *   `O`: Set if signed overflow occurred (Wraparound).

### 1.2 Multiplication (`MUL`)
*   **Input**: Two 27-trit words.
*   **Output**: Lower 27 trits of the product.
*   **Flags**: Updates `Z, P, N`.
*   **Overflow (`O`)**: Set if the mathematical result exceeds 27 trits (i.e., if upper word is non-zero).
*   **Sign Logic**: Balanced Ternary signs apply automatically.

### 1.3 Division (`DIV`)
*   **Input**: Dividend (Rs1), Divisor (Rs2).
*   **Output**: Quotient stored in `Rd`.
*   **Rounding**: **Toward Zero** (Truncation).
*   **Remainder**: Available in `STATUS.Aux` or requires `REM` instruction?
    *   *Decision*: Phase-1 adds `MOD` (Modulo) instruction for remainder.
*   **Sign Rule**: Remainder sign follows the Dividend.
    *   `10 / 3 = 3 rem 1`
    *   `-10 / 3 = -3 rem -1`
*   **Trap**: Divide by Zero triggers `#DIV0` exception.

---

## 2. Logic Operations
Ternary logic operates trit-wise on 27 trits in parallel.

### 2.1 Min (`AND`)
*   `T AND T = T`
*   `T AND 0 = T`
*   `0 AND 1 = 0`
*   `1 AND 1 = 1`
*   *Logic*: Result is the minimum value of inputs.

### 2.2 Max (`OR`)
*   `T OR T = T`
*   `0 OR T = 0`
*   `1 OR 0 = 1`
*   `1 OR 1 = 1`
*   *Logic*: Result is the maximum value of inputs.

### 2.3 Consensus (`XOR` equivalent)
*   Often called `MUL` (Logic Multiply) or `Consensus`.
*   Evaluates to `1` only if inputs are same?
*   **Helix9 Spec**: Use **Modulo-Sum** or **K-Map** XOR?
    *   *Decision*: **Ternary XOR (Sum Modulo 3)** without carry.
    *   `1 + 1 = T`
    *   `1 + T = 0`
    *   `0 + T = T`

---

## 3. Shift & Rotate
*   **LSL (Logical Shift Left)**: Shift in `0`s. Multiplies by 3.
*   **LSR (Logical Shift Right)**: Shift in `0`s. Divides by 3 (truncated).
*   **ASR (Arithmetic Shift Right)**: Duplicates MSB (Sign Trit).
*   **ROL (Rotate Left)**: Circular shift.

*Note: Phase-1 includes placeholders for these instructions.*

---

## 4. Comparison
*   **Instruction**: `CMP Rs1, Rs2`
*   **Action**: Performs `Rs1 - Rs2` internally.
*   **Update**: Sets `Z, P, N` flags based on result. Discards numeric output.
*   **Usage**: Follow with `B.EQ`, `B.GT`, `B.LT`.

---
*Verified & Frozen: Phase-1*
