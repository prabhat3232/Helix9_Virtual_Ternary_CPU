# Ternary Number System Specification
**Phase-1 Architecture**
**Version:** v0.1

---

## 1. Representation
Helix9 uses **Balanced Ternary** exclusively.
*   **Base**: 3
*   **Digits (Trits)**: `-1, 0, +1`
*   **Notation**: `T` (Trit -1), `0` (Zero), `1` (Trit +1)

### Why Balanced?
*   **Symmetry**: Positive and negative ranges are symmetrical around zero.
*   **No Sign Bit**: The sign is determined by the most significant non-zero trit.
*   **Cleaner Arithmetic**: Addition and subtraction use the exact same logic.

---

## 2. Data Types

### 2.1 Trit (Fundamental Unit)
*   **Values**: `{-1, 0, +1}`
*   **Binary Storage (Emulator)**: Usually stored as `int8_t` or packed 2-bit.

### 2.2 Tryte (Byte Equivalent)
*   **Width**: 6 Trits
*   **Range**: $-(3^6-1)/2$ to $+(3^6-1)/2$
*   **Decimal**: `-364` to `+364`
*   **Usage**: Character data, small counters, opcodes.

### 2.3 Word (Architecture Width)
*   **Width**: 27 Trits
*   **Range**: $-(3^{27}-1)/2$ to $+(3^{27}-1)/2$
*   **Decimal**: ~ `+/- 3.81 Trillion`
*   **Bits Equivalent**: $\log_2(3^{27}) \approx 42.79$ bits.
*   **Usage**: Pointers, Registers, Arithmetic, Addresses.

---

## 3. Arithmetic Rules

### 3.1 Addition Table
| A | B | Result | Carry |
| :---: | :---: | :---: | :---: |
| 0 | 0 | 0 | 0 |
| 1 | 0 | 1 | 0 |
| T | 0 | T | 0 |
| 1 | 1 | T | 1 |
| T | T | 1 | T |
| 1 | T | 0 | 0 |

*Note: `T` represents -1.*

### 3.2 Overflow Strategy
*   **Type**: Wraparound.
*   **Mechanism**: Carry out of the Most Significant Trit (MST) is discarded (but captured in STATUS.Carry flag).
*   **Example (3-trit)**:
    *   Max Value: `111` (+13)
    *   Add 1: `111 + 001`
    *   Result: `TTT` (-13) -> This is a geometric wraparound.

### 3.3 Multiplication
*   **Width**: `Word x Word = Word` (27 trits).
*   **Truncation**: The lower 27 trits of the result are kept.
*   **Overflow**: If the mathematical result exceeds the 27-trit range, the `STATUS.Overflow` flag is set.

---

## 4. Binary <-> Ternary Interaction (Emulator Level)

Since the hardware is virtual, binary interaction is strictly defined for I/O and emulation.

### 4.1 Storage Encoding
When storing Ternary data in Binary files (RAM dumps, executables):
*   **Format**: `2-bit packed`
*   **Mapping**:
    *   `00` -> `0`
    *   `01` -> `+1`
    *   `11` -> `-1` (T)
    *   `10` -> *Reserved / Padding*

### 4.2 API Conversion
*   **Int64 -> Word**: Precision loss if input > 42 bits. Emulated hardware captures simplified interactions.
*   **Word -> Int64**: Lossless conversion supported efficiently.

---

## 5. Immediate Values
Immediate values in instructions follow the **standard balanced ternary** rules.
*   **Sign Extension**: Unless specified otherwise (e.g., `LD.U`), immediates loaded into registers are sign-extended to 27 trits.
    *   *Example*: Loading narrow tryte `1T` (+2) into a register results in `00...001T` (+2).
    *   *Example*: Loading narrow tryte `T1` (-2) into a register results in `00...00T1` (-2). *Wait -- in balanced ternary, leading zeros preserve value regardless of sign. Leading `T`s would flip the magnitude.*
    *   **Correction**: In balanced ternary, "Sign Extension" implies padding with **Zeros**.
    *   **Rule**: To preserve value, always pad with `0`s to the left. The sign is inherent in the digits.

---
*Verified & Frozen: Phase-1*
