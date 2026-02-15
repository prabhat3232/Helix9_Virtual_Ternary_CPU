# HelixASM Relocation Model v1.0

## 1. Purpose
Relocations allow code to be assembled independently and linked together. The Assembler emits placeholders (0 or dummy values) and a Relocation Entry. The Linker calculates the final address and patches the code.

## 2. Relocation Types

### 2.1 R_HELIX_ABS (Absolute)
- **Usage**: Loading the address of a global variable.
- **Instruction**: `ldi.w r1, <TargetSymbol>`
- **Patch**: The entire 10-trit Immediate field is replaced with the absolute address of `TargetSymbol`.
- **Range**: ±29,524 (Limited by 10-trit immediate). *Note: Large addresses may require high/low split in future.*

### 2.2 R_HELIX_PCR (PC-Relative)
- **Usage**: Branches and Calls.
- **Instruction**: `bgt <TargetLabel>` or `call <TargetFunc>`
- **Calculation**: `Offset = TargetAddress - (PC + 1)`
- **Patch**: The 10-trit Immediate field is replaced with the calculated Offset.
- **Range**: ±29,524 words from current PC.

---

## 3. Workflow

1. **Assembler**: 
   - Sees `call MyFunc`. 
   - `MyFunc` is undefined (External).
   - Emits `call 0`.
   - Adds Relocation: `{ Offset: CurrPC, Sym: "MyFunc", Type: PCR }`.

2. **Linker**:
   - Merges object files. Determines `MyFunc` is at address 500.
   - Call site is at 100.
   - Calculates `Offset = 500 - (100 + 1) = 399`.
   - Patches instruction at 100 with `call 399`.
