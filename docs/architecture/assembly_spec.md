# Helix9 Assembly Language Specification (`.hasm`)
**Version**: 0.1  
**Status**: Experimental

## Overview
HelixASM is the text-based assembly language for the Helix9 Ternary CPU. It maps 1:1 with the v0.1 ISA but adds syntactic sugar for labels, data definition, and memory layout.

## Syntax
- **Case Sensitivity**: Mnemonics and Registers are Case-Insensitive (`ADD` = `add`). Labels are Case-Sensitive.
- **Comments**: Start with `;` and continue to end of line.
- **Whitespace**: Spaces and tabs are ignored except as separators.

### Line Format
```bnf
[Label:] [Mnemonic] [Operands] [; Comment]
```

### Examples
```asm
Start:              ; Label definition
    LDI R1, 10      ; Load Immediate
    ADD R3, R1, R2  ; R3 = R1 + R2
    JMP Start       ; Jump to Label
```

## Data Types & Literals
| Type | format | Example | Note |
|---|---|---|---|
| **Decimal** | `123`, `-45` | `LDI R1, -5` | Converted to balanced ternary |
| **Ternary** | `t(...)` | `t(+-0++)` | Raw trit specification |
| **Hex** | `0x...` | `0x1F` | *Future Extension* |
| **Char** | `'c'` | `'A'` | ASCII value to integer |

## Registers
- `R0` - `R11`: General Purpose
- `R12` (FP), `R13` (SP), `R14` (LR), `R15` (PC)
- `Status`: (Read-only via MRS, Write via MSR)

## Directives
Directives control the assembler and memory layout. They do not map to CPU instructions directly.

### `.ORG <address>`
Sets the memory address for subsequent instructions/data.
```asm
.ORG 100    ; Start assembling at address 100
```

### `.DAT <value>`
Emits a raw 27-trit word with the specified value.
```asm
MyVar: .DAT 42      ; Store decimal 42
Flag:  .DAT t(+--)  ; Store ternary value
```

## Instruction Set (v0.1)

### Arithmetic
| Mnemonic | Operands | Description |
|---|---|---|
| `ADD` | `Rd, Rs1, Rs2` | Rd = Rs1 + Rs2 |
| `SUB` | `Rd, Rs1, Rs2` | Rd = Rs1 - Rs2 |
| `MUL` | `Rd, Rs1, Rs2` | Rd = Rs1 * Rs2 |
| `DIV` | `Rd, Rs1, Rs2` | Rd = Rs1 / Rs2 |
| `MOD` | `Rd, Rs1, Rs2` | Rd = Rs1 % Rs2 |

### Logic
| Mnemonic | Operands | Description |
|---|---|---|
| `AND` | `Rd, Rs1, Rs2` | Min(Rs1, Rs2) |
| `OR`  | `Rd, Rs1, Rs2` | Max(Rs1, Rs2) |
| `XOR` | `Rd, Rs1` | *Placeholder* |

### Data Movement
| Mnemonic | Operands | Description |
|---|---|---|
| `MOV` | `Rd, Rs1` | Rd = Rs1 |
| `LDI` | `Rd, Imm` | Rd = Immediate |
| `LDW` | `Rd, [Rs1 + Imm]` | Load Word from Mem[Rs1 + Imm] |
| `STW` | `Rd, [Rs1 + Imm]` | Store Rd to Mem[Rs1 + Imm] |

### Control Flow
| Mnemonic | Operands | Description |
|---|---|---|
| `JMP` | `Target` | PC = Target (Label or Register) |
| `BEQ` | `Target` | Branch if Z=1 |
| `BNE` | `Target` | Branch if Z=0 |
| `BGT` | `Target` | Branch if P=1 |
| `BLT` | `Target` | Branch if N=1 |
| `CALL`| `Target` | LR=PC, PC=Target |
| `RET` | | PC=LR |

### System
| Mnemonic | Operands | Description |
|---|---|---|
| `HLT` | | Halt Execution |
| `NOP` | | No Operation |
| `MSR` | `Rs1` | Status = Rs1 |
| `MRS` | `Rd` | Rd = Status |

## assembler Pipeline
1.  **Pass 1**: Scan for Labels. Populate Symbol Table (Label -> Address).
2.  **Pass 2**: Parse Instructions. Resolve Labels. Emit Ternary Words.
3.  **Output**: `.ht` file (Helix Ternary Binary/Text format).
