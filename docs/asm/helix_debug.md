# Helix Debug Format (HDF) v0.1

## 1. Purpose
To enable source-level debugging (C-lite or ASM) on the Helix CPU Emulator.

## 2. Debug Information
Stored in a separate `.sym` file or a special `.debug` section in `.htx`.

### 2.1 Line Number Mapping
Maps `Address` -> `SourceFile:Line`.

```text
0x0100 -> main.c:5
0x0101 -> main.c:6
```

### 2.2 Symbol Table
Maps `Address` -> `Name`.

```text
0x0100 -> main
0x0250 -> my_global_var
```

### 2.3 Function Boundaries
Defines stack frame info for unwinding.
- Start Address
- End Address
- Frame Size (optional)

## 3. Emulator Integration
The CPU Emulator will load `.htx` for execution and `.sym` for the UI.
- On Trap/Halt: Show Source Line.
- `step`: Step by source line (using map).
