# Memory Model Specification
**Phase-1 Architecture**
**Version:** v0.1

---

## 1. Physical Memory Model

### 1.1 Addressing
Helix9 is a **Harvard-Modified** architecture (Unified Memory Bus for simplified emulation, but logically distinct code/data usage can be enforcing).
*   **Address Space**: 27 Trits (Word-Addressable).
*   **Addressable Unit**: **1 Word (27 Trits)**.
    *   Address `0` = Word 0
    *   Address `1` = Word 1
    *   *Note: This is different from Byte-Addressable CPUs. There are no "byte offsets".*

### 1.2 Endianness
**Big-Endian Ternary**
*   Within a Multi-Word structure, the **Lowest Address** holds the **Most Significant Word**.
*   *Note: Since memory is Word-Addressable, "Endianness" inside a word is opaque to the programmer unless accessing sub-trytes. Typically, Trit 26 is MSB, Trit 0 is LSB.*

### 1.3 Alignment
*   **Requirement**: Strict Alignment.
*   **Rule**: Since memory is word-addressable, all `LD/ST` operations are inherently word-aligned by definition.
*   *Trap*: Emulated hardware may trap if invalid addresses (out of range) are accessed.

---

## 2. Memory Map (Phase-1)

| Start Address (Tryte Hex) | End Address | Size | Function |
| :--- | :--- | :--- | :--- |
| `000000` | `000FFF` | ~4K Words | **ROM** (BIOS / Bootloader) |
| `001000` | `07FFFF` | ~500K Words | **RAM** (System Memory) |
| `080000` | `XXXXXX` | - | **MMIO** (Reserved for Peripherals) |
| `~~~~~~` | `~~~~~~` | - | **IVT** (Interrupt Vector Table is at 0x000004 usually) |

*Note: Addresses are represented in compacted Tryte-Hex for readability.*

---

## 3. Access Semantics

### 3.1 Load Instructions
*   **`LDW` (Load Word)**: Loads 27 trits from memory `M[Addr]` to `Rd`.
*   **`LDT` (Load Trit)**: Loads 27 trits, but treats source as containing only LS-Trit?
    *   *Correction for Word-Addressing*: To load partial data (Trit/Tryte), the CPU loads the full word, shifting/masking must be handled by software OR specific `LD.T` instruction.
    *   **Phase-1 Spec**: `LD.T` loads a Word, isolates the LSB Trit, and **Sign-Extends** it to fill the register.
    *   **Phase-1 Spec**: `LD.U` loads a Word, isolates the LSB Trit, and **Zero-Pads** it.

### 3.2 Store Instructions
*   **`STW` (Store Word)**: Writes 27 trits from `Rs` to `M[Addr]`.
*   **`STT` (Store Trit)**: Read-Modify-Write operation. Reads `M[Addr]`, updates LSB Trit from `Rs`, writes back.
    *   *Performance Warning*: `STT` and `STK` (Store Tryte) are slower than `STW`.

### 3.3 Protection
Phase-1 implements basic **Read/Write/Execute** (RWX) protection flags per Memory Page (emulated).
*   **ROM**: RX (Read/Execute, No Write).
*   **RAM**: RW (Read/Write, No Execute - NX bit optional).
*   **Violation**: Triggers `Memory Fault` Exception.

---
*Verified & Frozen: Phase-1*
