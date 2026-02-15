# Register File & ABI Specification
**Phase-1 Architecture**
**Version:** v0.1

---

## 1. Register Architecture

Helix9 provides **16 General Purpose Registers (GPRs)**.
*   **Width**: 27 Trits (1 Word)
*   **Access**: Single-cycle read/write/accumulate.

| Register | Name | Hardware Role | ABI Role (Software Convention) |
| :--- | :--- | :--- | :--- |
| **R0** | - | General | **Arg0 / Ret0** (First Argument / Return Value) |
| **R1** | - | General | **Arg1 / Ret1** (Second Argument / High Ret) |
| **R2** | - | General | **Arg2** |
| **R3** | - | General | **Arg3** |
| **R4** | - | General | **Caller-Saved** (Temp) |
| **R5** | - | General | **Caller-Saved** (Temp) |
| **R6** | - | General | **Callee-Saved** (Saved) |
| **R7** | - | General | **Callee-Saved** (Saved) |
| **R8** | - | General | **Callee-Saved** (Saved) |
| **R9** | - | General | **Callee-Saved** (Saved) |
| **R10** | - | General | **Callee-Saved** (Saved) |
| **R11** | - | General | **Callee-Saved** (Saved) |
| **R12** | **FP** | Frame Pointer | Anchors the current stack frame. |
| **R13** | **SP** | Stack Pointer | Points to the **Top of Stack** (Next free word). |
| **R14** | **LR** | Link Register | Stores return address during `CALL`. |
| **R15** | **PC** | Program Counter | Current Instruction Address. |

---

## 2. Special Registers

### 2.1 STATUS Register
A 27-trit read-only register (writable only via `MSR` instruction) tracking processor state.

| Bit/Trit Index | Flag | Name | Meaning |
| :--- | :--- | :--- | :--- |
| 0 | **Z** | Zero | Last result was `0`. |
| 1 | **P** | Positive | Last result was `> 0`. |
| 2 | **N** | Negative | Last result was `< 0`. |
| 3 | **C** | Carry | Arithmetic carry occurred. |
| 4 | **O** | Overflow | Arithmetic result exceeded capacity. |
| 5 | **IM** | Interrupt Mask | `0`=Disabled, `1`=Enabled. |
| 6-26 | - | Reserved | Reserved for future use. |

*Note: Flags are typically represented as Trits. `1`=Set, `0`=Clear.*

---

## 3. Calling Convention (ABI)

### 3.1 Function Calls
*   **First 4 Arguments**: Passed in `R0` - `R3`.
*   **Excess Arguments**: Pushed onto the Stack (Right-to-Left).
*   **Return Value**:
    *   Scalar (Word or less): `R0`
    *   Double Word: `R0` (Low), `R1` (High)

### 3.2 Stack Frame
The stack grows **Downwards** (High Address -> Low Address).
`SP` points to the *last pushed item* (Full Descending) OR *next free slot*?
*   **Decision**: **Full Descending** (`SP` points to valid data).
    *   *Push*: Decrement `SP`, then Store.
    *   *Pop*: Load, then Increment `SP`.
*   **Alignment**: Stack Pointer must **always** be Word-Aligned (27 trits).

**Frame Layout:**
```
High Addr
   ...         (Previous Stack)
   [Arg N]
   [Arg 4]
   [Ret Addr]  (Saved LR)
   [Old FP]    (Saved R12)
   [Locals]    <-- FP (points here)
   ...
Low Addr       <-- SP (points here)
```

---

## 4. Reset & Boot
On Power-Up or Reset:
*   `PC` -> `0x0000_0000` (ROM Entry)
*   `SP` -> `RAM_TOP` (End of Physical RAM)
*   `STATUS` -> `0` (Interrupts Disabled)
*   All GPRs -> `0`

---
*Verified & Frozen: Phase-1*
