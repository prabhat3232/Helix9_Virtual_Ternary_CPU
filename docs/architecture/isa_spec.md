# Helix9 ISA Specification v1.0
**Phase-1 Architecture**
**Version:** v0.1 **(Draft Frozen)**

---

## 1. Instruction Format
Fixed Width: **27 Trits**

| Field | Width (Trits) | Description |
| :--- | :--- | :--- |
| **Opcode** | 6 | Identifies operation (Space for ~700 ops) |
| **Mode** | 3 | Addressing Mode / Sub-Opcode |
| **Rd** | 4 | Destination Register (0-15) |
| **Rs1** | 4 | Source Register 1 |
| **Rs2 / Imm** | 10 | Source Register 2 OR Immediate Value |

*Note: 4 trits can address 81 registers. We only use values 0-15.*
*Note: 10 trits Immediate = range +/- 29,524.*

---

## 2. Instruction Categories

### 2.1 Arithmetic (ALU)
| Mnemonic | Name | Flag Updates |
| :--- | :--- | :--- |
| `ADD` | Add | Z, P, N, O, C |
| `SUB` | Subtract | Z, P, N, O, C |
| `MUL` | Multiply | Z, P, N, O |
| `DIV` | Divide | Z, P, N, O |
| `MOD` | Modulo | Z, P, N |

### 2.2 Logic
| Mnemonic | Name | Flag Updates |
| :--- | :--- | :--- |
| `AND` | Min | Z |
| `OR` | Max | Z |
| `XOR` | Consensus | Z |
| `LSL` | Logic Shift Left | Z, C |
| `LSR` | Logic Shift Right | Z |

### 2.3 Data Movement
| Mnemonic | Name | Description |
| :--- | :--- | :--- |
| `LDW` | Load Word | `Rd <- M[Rs + Imm]` |
| `STW` | Store Word | `M[Rs + Imm] <- Rd` |
| `MOV` | Move | `Rd <- Rs` |
| `LDI` | Load Immediate | `Rd <- Imm` |

### 2.4 Control Flow
| Mnemonic | Name | Condition |
| :--- | :--- | :--- |
| `JMP` | Jump | Unconditional |
| `BEQ` | Branch Equal | `Z == 1` |
| `BNE` | Branch Not Equal | `Z == 0` |
| `BGT` | Branch Greater | `P == 1` |
| `BLT` | Branch Less | `N == 1` |
| `CALL` | Call Subroutine | Push LR, Jump |
| `RET` | Return | Pop LR, Jump |

### 2.5 System
| Mnemonic | Name | Description |
| :--- | :--- | :--- |
| `NOP` | No Operation | `PC++` |
| `HLT` | Halt | Stop Pipeline |
| `MSR` | Move to Status | `STATUS <- Rs` |
| `MRS` | Move from Status | `Rd <- STATUS` |

---

## 3. Opcode Encoding (Preliminary)
*Opcode Maps are reserved for the Assembler Implementation phase, but we define the blocks here.*

*   **00xxxx**: System / Control
*   **01xxxx**: Arithmetic
*   **02xxxx**: Logic
*   **10xxxx**: Memory
*   **11xxxx**: Reserved (AI Extensions)

---
*Verified & Frozen: Phase-1*
