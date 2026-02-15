# Helix Linker Plan

## 1. Goal
Create a minimal linker (`helix_ld`) that combines multiple `.ht` object files into a single flat binary (or executable `.htx`).

## 2. Responsibilities
1.  **Section Merging**: Concatenate all `.text` sections and all `.data` sections.
2.  **Symbol Resolution**: Gather all exported symbols (GLOBAL) and resolve imports (EXTERN). Error on undefined or duplicate symbols.
3.  **Relocation Application**: iterated through logical sections, applying rigid math to patch instruction immediates.
4.  **Entry Point Definition**: Default to `main` or specific start symbol.

## 3. Memory Map (v0.1 Default)
- **ROM (Text)**: Starts at `0x100` (Address 100).
- **RAM (Data)**: Starts at `0x1000` (Address 2187 approx).
- **Stack**: Top of Memory (growing down).

## 4. Implementation Steps
1.  **Reader**: Parse `.ht` format.
2.  **Symbol Table Builder**: Map `Name -> FinalAddress`.
3.  **Patcher**: Apply relocations.
4.  **Writer**: Emit final executable.
