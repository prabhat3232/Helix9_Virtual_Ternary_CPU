# Helix Binary Format (.ht) v1.0

## 1. File Structure
The `.ht` (Helix Ternary) file is a relocatable object file used by the Linker and Loader.
It is a textual representation (for v0.1) that maps logically to a binary structure.

### Layout
```
[ HEADER ]
[ SECTION HEADER TABLE ]
[ SECTION DATA: .text ]
[ SECTION DATA: .data ]
[ SYMBOL TABLE ]
[ RELOCATION TABLE ]
```

---

## 2. Header
| Field | Value | Description |
| :--- | :--- | :--- |
| Magic | `HTX` | File Identifier |
| Version | `1` | Format Version |
| Entry | `Address/Sym` | Entry Point if executable |
| NumSections | `Integer` | Count of sections |

---

## 3. Sections
Common sections:
- `.text`: Executable code (ReadOnly).
- `.data`: Initialized globals (ReadWrite).
- `.bss`: Uninitialized globals (Zeroed).
- `.rodata`: Constants (ReadOnly).

---

## 4. Symbol Table
Maps names to addresses/sections.

| Field | Description |
| :--- | :--- |
| Name | String identifier |
| Section | Index of section definition |
| Offset | Offset within section |
| Scope | LOCAL / GLOBAL |

---

## 5. Relocation Table
Directs the linker how to patch code.

| Field | Description |
| :--- | :--- |
| Offset | Where to patch (Section + Offset) |
| Symbol | Symbol to reference |
| Type | ABSOLUTE or PC_RELATIVE |
