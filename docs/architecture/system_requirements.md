# Helix9 System Requirements Specification (SRS)
**Phase-1 Architecture Frozen Baseline**
**Version:** v0.1-draft
**Status:** Experimental / Research

---

## 1. System Overview
**Helix9** is a virtual ternary processor designed for high-performance sparse AI workloads.
This document defines the mandatory requirements for the **Phase-1 implementation** (virtual CPU).

### 1.1 Philosophy
*   **Minimalism**: Only essential instructions and features.
*   **Compiler-First**: Regular, orthogonal instruction set.
*   **Math Consistency**: Balanced Ternary arithmetic without "signed/unsigned" duality.
*   **AI-Native**: Built for sparse vector operations.

---

## 2. Core Architecture Constants
The following constants are **immutable** for v1.0.

*   **Radix**: Balanced Ternary (Trits: `T(-1), 0, 1`)
*   **Word Size**: 27 Trits (~42.79 bits)
*   **Address Space**: 27 Trits (Word-Addressable)
*   **Instruction Width**: Fixed 27 Trits
*   **Endianness**: Big-Endian Ternary (Lowest Address = Most Significant Trit)

| Unit  | Width | Range (Decimal Approx) |
| :--- | :--- | :--- |
| **Trit** | 1 | -1 to +1 |
| **Tryte** | 6 | -364 to +364 |
| **Word** | 27 | -3.81 Trillion to +3.81 Trillion |

---

## 3. Programming Model

### 3.1 Register File
16 General Purpose Registers (GPR), 27 trits wide.

| Register | Role | Description |
| :--- | :--- | :--- |
| **R0 - R11** | General | Available for arithmetic, logic, data pointers. |
| **R12** | FP | Frame Pointer (for stack frames). |
| **R13** | SP | Stack Pointer (Points to Top-of-Stack). |
| **R14** | LR | Link Register (Return address). |
| **R15** | PC | Program Counter. |

### 3.2 System Registers
*   **STATUS**: 27-trit register containing flags and mode bits.
    *   **Z** (Zero), **P** (Positive), **N** (Negative)
    *   **O** (Overflow), **C** (Carry)
    *   **IM** (Interrupt Mask)
    *   *Writable only via special `MSR` instruction.*

---

## 4. Memory System

### 4.1 Addressing
*   **Granularity**: Word-Addressable (1 Address = 1 Word).
*   **Alignment**: All memory accesses must be word-aligned.
    *   *Violation Requirement*: Trap on misaligned access.

### 4.2 Load/Store Semantics
*   **Atomic Unit**: Word (27 trits).
*   **Partial Loads**:
    *   `LD.T` (Load Trit) -> Sign-Extend to Word.
    *   `LD.U` (Load Trit Unsigned) -> Zero/Pad-Extend to Word.
*   **Store**: Partial stores allowed for Trit/Tryte.

### 4.3 Memory Map (Phase-1 Default)
*   `0x0000_0000...` : **ROM** (Boot Code)
*   `0x0000_1000...` : **RAM** (Main Memory)
*   `0xMMMM_MMMM...` : **MMIO** (Reserved for Peripherals)

---

## 5. Arithmetic & Logic

### 5.1 Native Arithmetic
*   **Representation**: Balanced Ternary (Standard).
*   **Overflow Strategy**: Wraparound.
    *   Example: `MaxWord + 1 = MinWord`
*   **Carry Propagation**: `-1, 0, +1` capable.

### 5.2 Operations
*   **Addition / Subtraction**: Standard balanced math. Updates `Z, P, N, O, C`.
*   **Multiplication**: Result Truncated to 27 trits (Lower word). Wraparound behavior.
*   **Division**:
    *   Rounding: Toward Zero.
    *   Remainder Sign: Follows Dividend.
    *   *Equation*: `Dividend = (Divisor * Quotient) + Remainder`
    *   *Trap*: Divide-by-Zero.

### 5.3 Logic
*   **Ternary Gates**: Min (`AND`), Max (`OR`), Consensus (`XOR`-equivalent).
*   **Shifts**: Logical Shift, Arithmetic Shift, Rotate (Placeholders).

---

## 6. Execution Model

### 6.1 Guarantees
*   **Model**: In-Order, Single-Core.
*   **Determinism**: Guaranteed for identical instruction streams (given identical memory state & interrupts disabled).
*   **Pipeline Visibility**: NONE (Architecturally visible state updates atomically).

### 6.2 Control Flow
*   **PC Update**: Increments by 1 (word) after Fetch.
*   **Branch Resolution**: Overrides PC at Decode/Execute stage.
*   **Comparison**:
    *   `CMP` Instruction sets `Z, P, N` flags based on result.
    *   Branch Instructions (`BEQ`, `BGT`, `BLT`) **READ ONLY** flags.

### 6.3 Exceptions & Traps
System uses a **Trap-Based** exception model.

| Trap Type | Trigger | Action |
| :--- | :--- | :--- |
| **Illegal Opcode** | Standard | Jump to Vector Table |
| **Divide By Zero** | `DIV` with 0 divisor | Jump to Vector Table |
| **Memory Fault** | Invalid Address | Jump to Vector Table |
| **Misaligned Access** | Unaligned Load/Store | Jump to Vector Table |
| **Stack Fault** | SP Misalignment | Jump to Vector Table |

**Reset State**:
*   `PC` = `0` (ROM Start)
*   `SP` = `RAM_TOP`
*   `Regs` = `0`
*   `STATUS` = `0` (Interrupts Disabled)

---

## 7. Calling Convention (ABI) Phase-1

*   **Stack Growth**: Descending (Full Descending).
*   **Alignment**: Stack Pointer (SP) must be 27-trit word aligned at all times.
*   **Argument Passing**: Registers `R0-R3`, then Stack.
*   **Return Values**: `R0` (and `R1` for wide results).

---
*Verified & Frozen: 2026-02-15*
