# Execution Model Specification
**Phase-1 Architecture**
**Version:** v0.1

---

## 1. Pipeline Model (Conceptual)

Helix9 is defined as an **In-Order** architecture. While the emulator may optimize, the architectural state updates must appear atomic and sequential.

### 1.1 Stages
1.  **Fetch**: Retrieve instruction Word from `M[PC]`. Increment `PC` by 1.
2.  **Decode**: Parse format, read source registers.
3.  **Execute**: Perform ALU op, Memory access, or Branch.
4.  **Writeback**: Update destination register `Rd` or `STATUS`.

### 1.2 Program Counter (PC)
*   **Behavior**: `PC` tracks the *current* instruction.
*   **Update Rule**:
    *   During **Fetch**: `PC <- PC + 1`
    *   During **Execute** (Branch Taken): `PC <- Target Address`
*   **Note**: This means `R15` (PC) read during execution usually points to `Current + 1`. The assembler handles offsets.

### 1.3 Determinism
Helix9 guarantees **Bit-Exact Determinism** under these conditions:
1.  Single-threaded execution.
2.  Interrupts Disabled (`STATUS.IM = 0`).
3.  Identical initial Memory State.

---

## 2. Interrupts & Exceptions

### 2.1 Trap-Based Model
Hardware events trigger a forced jump to a predefined **Vector Table**.

### 2.2 Vector Table (Phase-1)
Located at the beginning of Memory (ROM):
| Address | Vector Name | Trigger |
| :--- | :--- | :--- |
| `0x0000` | **RESET** | Power-on / Reset |
| `0x0001` | **ILLEGAL** | Unknown Opcode |
| `0x0002` | **DIV0** | Arithmetic Error |
| `0x0003` | **MEM_FAULT** | Invalid Address / Write-to-ROM |
| `0x0004` | **ALIGN_FAULT** | Misaligned Access |
| `0x0005` | **IRQ** | Hardware Interrupt (Timer/IO) |

### 2.3 Exception Sequence
When a trap occurs:
1.  Current `PC` is saved to `EPC` (Exception PC - hidden SR).
    *   *Alternative*: Pushed to Stack? **Decision**: **Push to Stack** to allow nesting.
2.  `STATUS` is Pushed to Stack.
3.  `STATUS.IM` is set to `0` (Disable Interrupts).
4.  `PC` is loaded with Vector Address.

### 2.4 Interrupt Masking
*   **Enable**: Set `STATUS.IM = 1` via `MSR` instruction.
*   **Disable**: Clear `STATUS.IM = 0`.
*   **Default**: Disabled on Reset.

---

## 3. System Control
*   **NOP**: `NOP` instruction guarantees no state change (except PC increment) and consumes 1 cycle equivalent.
*   **Wait/Halt**: `HLT` instruction stops fetches until Interrupt occurs.

---
*Verified & Frozen: Phase-1*
