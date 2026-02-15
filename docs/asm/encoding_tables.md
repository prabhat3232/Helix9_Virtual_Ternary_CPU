# HelixASM Binary Encoding v1.0 (FROZEN)

## 1. Instruction Format (27-Trit)

All instructions are fixed-width 27-trit words.
Layout:
```
[ Opcode (6) ] [ Mode (3) ] [ Rd (4) ] [ Rs1 (4) ] [ Rs2/Imm (10) ]
```
Total: 6 + 3 + 4 + 4 + 10 = 27 trits.

### Fields
- **Opcode**: 6 trits. Identifies operation (`add.w`, `ld.w`).
- **Mode**: 3 trits. Defines addressing mode (Register, Immediate, Indexed).
- **Rd**: 4 trits. Destination Register Index (0-15).
- **Rs1**: 4 trits. Source Register 1.
- **Rs2/Imm**: 10 trits. Source Register 2 or Immediate Value (±29,524 range).

---

## 2. Opcode Map

| Mnemonic | Opcode (Ternary) | Opcode (Decimal) |
| :--- | :--- | :--- |
| `halt` | `000000` | 0 |
| `nop`  | `000001` | 1 |
| `add.w`| `00000T` | 2 |
| `sub.w`| `000010` | 3 |
| `mul.w`| `000011` | 4 |
| `div.w`| `00001T` | 5 |
| `mod.w`| `0000T0` | 6 |
| `and.w`| `0000T1` | 7 |
| `or.w` | `0000TT` | 8 |
| `xor.w`| `000100` | 9 |
| `lsl.w`| `000101` | 10 |
| `lsr.w`| `00010T` | 11 |
| `mov.w`| `000110` | 12 |
| `ldi.w`| `000111` | 13 |
| `ld.w` | `00011T` | 14 |
| `st.w` | `0001T0` | 15 |
| `jmp`  | `0001T1` | 16 |
| `beq`  | `0001TT` | 17 |
| `bne`  | `001000` | 18 |
| `bgt`  | `001001` | 19 |
| `blt`  | `00100T` | 20 |
| `call` | `001010` | 21 |
| `ret`  | `001011` | 22 |
| `msr`  | `00101T` | 23 |
| `mrs`  | `0010T0` | 24 |
| `cmp.w`| `0010T1` | 25 |

---

## 3. Addressing Modes (Mode Field)

| Mode | Value | Syntax |
| :--- | :--- | :--- |
| **Register** | `0` | `add.w r1 r2 r3` |
| **Immediate** | `1` | `add.w r1 r2 5` |
| **Direct Mem**| `2` | `ld.w r1 [r2]` |
| **Indexed Mem**| `3` | `ld.w r1 [r2+5]` |
| **PC Relative**| `4` | `beq label` |

---

## 4. Immediate Encoding (Rs2/Imm Field)

- **Width**: 10 trits.
- **Range**: -29,524 to +29,524 (Balanced Ternary).
- **Format**: Standard balanced ternary encoding.

## 5. Branch Offsets

- **Width**: 10 trits (embedded in Rs2/Imm).
- **Scaling**: Multiplied by 1 (Word alignment is implicit).
- **Relative**: Target Address = PC + Offset.
