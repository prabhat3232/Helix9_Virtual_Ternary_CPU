# HelixASM Specification v1.0 (FROZEN)

## 1. Philosophy
HelixASM is a **Compiler-First Intermediate Representation (IR)**.
It prioritizes parsing speed, ambiguity reduction, and machine-mapping over human readability.
It is the "Ground Truth" interface between software (Compiler) and hardware (CPU).

**Key Principles:**
- **Lowercase Mnemonics**: `add`, not `ADD`.
- **Suffix-Typing**: `add.w` (Word) explicitly defined.
- **Dest-First**: `op dest src1 src2` (Matches hardware writeback).
- **No Commas**: Whitespace separation only.
- **Explicit Memory**: `[base+offset]`.

---

## 2. Syntax

### 2.1 Instruction Format
```bnf
<mnemonic>.<width> <dest> <src1> <src2>
```

**Example:**
```asm
add.w r1 r2 r3      ; R1 = R2 + R3
sub.w r4 r4 1       ; R4 = R4 - 1
ld.w  r1 [r2]       ; Load R1 from Mem[R2]
st.w  r1 [r3+2]     ; Store R1 to Mem[R3+2]
cmp.w r1 r2         ; Compare R1, R2
bgt   loop          ; Branch if Greater
call  func          ; Call Function
ret                 ; Return
```

### 2.2 Operands
- **Registers**: `r0`..`r15` (Lowercase).
- **Immediates**: Decimal integers (`123`, `-10`).
- **Memory**: `[reg]` or `[reg+offset]`. Offset is immediate.

---

## 3. Instruction Set (Canonical)

### 3.1 Arithmetic
| Syntax | Semantics |
| :--- | :--- |
| `add.w rd r1 r2` | Rd = R1 + R2 |
| `sub.w rd r1 r2` | Rd = R1 - R2 |
| `mul.w rd r1 r2` | Rd = R1 * R2 |
| `div.w rd r1 r2` | Rd = R1 / R2 |
| `mod.w rd r1 r2` | Rd = R1 % R2 |

### 3.2 Logic
| Syntax | Semantics |
| :--- | :--- |
| `and.w rd r1 r2` | Rd = Min(R1, R2) |
| `or.w  rd r1 r2` | Rd = Max(R1, R2) |
| `xor.w rd r1 r2` | Rd = SumMod3(R1, R2) |
| `lsl.w rd r1`    | Rd = R1 << 1 |
| `lsr.w rd r1`    | Rd = R1 >> 1 |

### 3.3 Data Movement
| Syntax | Semantics |
| :--- | :--- |
| `mov.w rd r1`    | Rd = R1 |
| `ldi.w rd imm`   | Rd = Immediate |
| `ld.w  rd [rs]`      | Rd = Mem[Rs] |
| `ld.w  rd [rs+imm]`  | Rd = Mem[Rs + Imm] |
| `st.w  src [dst]`    | Mem[Dst] = Src |
| `st.w  src [dst+imm]`| Mem[Dst + Imm] = Src |

### 3.4 Control Flow
| Syntax | Semantics |
| :--- | :--- |
| `jmp target`     | PC = Target |
| `beq target`     | Branch if Z=1 |
| `bne target`     | Branch if Z=0 |
| `bgt target`     | Branch if P=1 |
| `blt target`     | Branch if N=1 |
| `call target`    | LR=PC, PC=Target |
| `ret`            | PC=LR |

### 3.5 System
| Syntax | Semantics |
| :--- | :--- |
| `halt`           | Stop CPU |
| `nop`            | No Op |

---

## 4. Directives

| Directive | Description |
| :--- | :--- |
| `.section name` | Start section (.text, .data) |
| `.global name`  | Export symbol |
| `.word value`   | Emit 27-trit literal |
| `.org value`    | Set address |

**Example:**
```asm
.section .data
val: .word 42

.section .text
.global main
main:
    ld.w r1 [val]
    halt
```

---

## 5. Binary Encoding (v1.0 Frozen)
*See `encoding_tables.md` for bit-level layout.*
(To be defined in next document)
