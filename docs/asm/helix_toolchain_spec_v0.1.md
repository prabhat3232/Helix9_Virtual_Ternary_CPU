# Helix-9 Toolchain Specification (v0.1)

> **Status**: Frozen
> **Version**: 0.1
> **Date**: 2026-02-15

## 1. Overview
The Helix-9 Toolchain v0.1 consists of three core components designed to process transition from Assembly source code to executables running on the Virtual Ternary CPU.

1. **HelixASM (`helix_asm`)**: Assembler. Compiles `.hasm` -> `.ht` (Relocatable Object).
2. **HelixLD (`helix_ld`)**: Linker. Combines `.ht` files -> `.hx` (Executable).
3. **HelixEMU (`helix_emu`)**: Emulator. Loads `.hx` and executes on virtual CPU.

---

## 2. File Formats

### 2.1 Relocatable Object File (`.ht`)
Text-based format for object code.

**Structure:**
```
HTX 1 <NumSections>
SECTION <Name> <BaseAddr> <Size>
<Word_Int64> <Word_Int64> ...
SECTION ...
...
SYMBOLS <Count>
<Name> <Section> <Offset> <Scope>
...
RELOCATIONS <Count>
<Offset> <Symbol> <Type> <Section>
...
```

- **Scope**: `G` (Global), `L` (Local).
- **Relocation Types**:
  - `ABS`: Absolute Address (e.g., `LDI R1, Label`). Patch lower 10 trits.
  - `PCR`: PC-Relative (e.g., `CALL Func`). Patch lower 10 trits with `Target - (PatchAddr + 1)`.

### 2.2 Executable File (`.hx`)
Final linked binary format loaded by the Emulator.

**Structure:**
```
HX 1 <NumSections>
SECTION <Name> <StartAddress> <Size>
<Word_Int64> <Word_Int64> ...
...
```

- **Address Space**: Linker flattens sections. Typically `.text` starts at 0, `.data` follows immediately.
- **Resolution**: All symbols and relocations are resolved to absolute or relative values baked into the instruction words.

---

## 3. Instruction Encoding (27-Trit)
The CPU executes fixed-width 27-trit instructions using Balanced Ternary logic.

**Format:**
`[Opcode: 6] [Mode: 3] [Rd: 4] [Rs1: 4] [Rs2/Imm: 10]`

| Field | Trits | Description |
|-------|-------|-------------|
| **Opcode** | 21-26 | Operation Identifier (see ISA). |
| **Mode** | 18-20 | Addressing Mode (0=Reg, 1=Imm, 2=MemDirect, 3=MemIndexed, 4=PCR). |
| **Rd** | 14-17 | Destination Register (R0-R15). |
| **Rs1** | 10-13 | Source Register 1. |
| **Rs2/Imm**| 0-9 | Source Register 2 OR Immediate Value (10 trits). |

### Addressing Modes
| Mode | Value | Description | Operand Syntax |
|------|-------|-------------|----------------|
| **Register** | 0 | `Rd = Op(Rs1, Rs2)` | `ADD R1, R2, R3` |
| **Immediate** | 1 | `Rd = Op(Rs1, Imm)` | `LDI R1, 42` |
| **Mem Direct** | 2 | `Rd = Mem[Rs1 + Imm]` | `LD.W R1, [R2]` (Imm=0) |
| **Mem Indexed**| 3 | `Rd = Mem[Rs1 + Imm]` | `LD.W R1, [R2+offset]` |
| **PC Relative**| 4 | `PC = PC + Imm` | `JMP Label`, `CALL Label` |

---

## 4. Workflows

### 4.1 Compilation
```bash
helix_asm main.hasm -o main.ht
helix_asm lib.hasm -o lib.ht
```

### 4.2 Linking
```bash
helix_ld main.ht lib.ht -o app.hx
```
**Linker Logic:**
1. **Merge**: Combines `.text` and `.data` sections from all input files.
2. **Layout**: Calculates final absolute addresses for all sections (`.text` at 0).
3. **Resolve**:
   - Maps Global symbols to absolute addresses.
   - Rejects Duplicate Global symbols.
   - Reports Undefined symbols.
4. **Patch**: Applies `ABS` and `PCR` relocations to the code.
   - **Fix**: Patches only the lower 10 trits (Immediate field) while preserving Opcode/Mode.

### 4.3 Execution
```bash
helix_emu app.hx [max_cycles]
```
- Loads `.hx` into `TernaryMemory`.
- Resets CPU (`PC=0`).
- Executes until `HALT` or `max_cycles`.

---

## 5. Verified Features (Phase 5)
- **Cross-File Calls**: Verified via `test_e2e` (PCR relocation).
- **Global Variables**: Verified data sharing (ABS relocation).
- **Complex Control Flow**: Verified Loops and Conditional Branching (`BGT`, `CMP`).
- **Memory Operations**: Verified `LD.W`/`ST.W` with offsets.
- **I/O**: Basic Store/Load validation.
