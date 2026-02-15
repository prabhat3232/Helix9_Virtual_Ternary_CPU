# Helix C-lite v0.1 — FINAL FREEZE SPEC

## 1. Purpose

Helix C-lite is a minimal, C-like high-level language targeting the Helix9 ternary CPU.
It compiles to HelixASM as its intermediate representation.

Design goals:
* deterministic behavior
* tiny compiler surface
* hardware-first semantics
* no dynamic/runtime complexity

Out of scope for v0.1:
* floats
* structs
* heap allocation
* pointers in language surface
* arrays
* exceptions

---

## 2. Data Types

### 2.1 Primitive Types

```c
int   : signed 27-trit integer
void  : no value
```

### 2.2 Integer Model

* Range: ±3,812,798,742,493 (27 trits)
* Representation: 27-trit signed word
* Overflow: wraps modulo 3^27
* No overflow traps

---

## 3. Memory Model

### 3.1 Addressing

* Word-addressed memory
* One word = one 27-trit value
* Pointers not exposed in language surface

### 3.2 Layout

```
ROM  → program text
RAM  → globals (.DAT)
STACK → downward growing
```

### 3.3 Alignment

* `int` aligned to 1 word
* stack word aligned

### 3.4 Endianness

Ternary word ordered, architecture-defined.
No byte addressing in v0.1.

---

## 4. Variables

### 4.1 Declaration

```c
int a;
int b = 10;
```

### 4.2 Storage Classes

| Type     | Location          |
| -------- | ----------------- |
| Global   | `.DAT` RAM        |
| Local    | Stack             |
| Register | Compiler decision |

### 4.3 Scope Rules

* block scope `{ }`
* shadowing allowed
* lifetime ends on block exit (Stack pop)

---

## 5. Functions

### 5.1 Definition

```c
int add(int x, int y) {
    return x + y;
}
```

### 5.2 Requirements

* must be declared before use
* implicit declarations not allowed
* recursion allowed

### 5.3 Return Rules

* non-void must explicitly return
* reaching end without return → undefined behavior

---

## 6. Program Entry

```c
int main()
```

* no arguments supported
* return value placed in R1
* runtime halts after return

---

## 7. ABI (Calling Convention)

### 7.1 Registers

| Purpose      | Register      |
| ------------ | ------------- |
| Arg1–Arg6    | R1–R6         |
| Return       | R1            |
| Callee-saved | R7–R11        |
| FP           | R12           |
| SP           | R13           |
| LR           | R14 (Link Register) |
| PC           | R15           |

### 7.2 Stack Frame

```
[ high address ]
arg spill (args > 6)
return addr (LR)
saved FP (Old R12)
locals
saved R7–R11
[ SP ↓ ]
```

Stack grows downward.

---

## 8. Control Flow

### 8.1 If / Else

```c
if (cond) { }
else { }
```

### 8.2 While

```c
while (cond) { }
```

*No `for`, `switch`, or `break` in v0.1.*

---

## 9. Boolean Model

| Value | Meaning |
| ----- | ------- |
| 0     | false   |
| != 0  | true    |

**Condition evaluation:**
* `x != 0` → true
* `x == 0` → false

**Comparison operators always return:**
* `1` → true
* `0` → false

---

## 10. Operators

### 10.1 Arithmetic

| Operator | Mapping |
| -------- | ------- |
| +        | ADD     |
| -        | SUB     |
| *        | MUL     |
| /        | DIV     |
| %        | MOD     |

*Division/modulo by zero → runtime trap.*

### 10.2 Ternary Logic

| Operator | Meaning   | Mapping |
| -------- | --------- | ------- |
| &        | min       | AND     |
| \|       | max       | OR      |
| ^        | sum mod 3 | XOR     |

### 10.3 Shifts

| Operator   | Behavior        |
| ---------- | --------------- |
| <<         | multiply by 3^n |
| >>         | divide by 3^n   |
| *Zero fill.* |                 |

### 10.4 Comparison

| Operator | Lowering  |
| -------- | --------- |
| ==       | CMP + BEQ |
| !=       | CMP + BNE |
| >        | CMP + BGT |
| <        | CMP + BLT |
| >=       | CMP + BGE |
| <=       | CMP + BLE |

---

## 11. Unary Operators

| Operator | Meaning                        |
| -------- | ------------------------------ |
| -        | arithmetic negation            |
| !        | logical NOT (0->1, else->0)    |
| ~        | defined as arithmetic negation |

---

## 12. Assignment

```c
a = b = 5;
```

Rules:
* right associative
* expression returns assigned value

---

## 13. Evaluation Order

Expressions evaluated **left → right**.
Example: `f() + g()` → `f` runs first.

---

## 14. Modulo Semantics

Sign follows dividend:
* `10 % 3  = 1`
* `-10 % 3 = -1`

---

## 15. Pointer Model

Language surface:
* no pointers
* no address-of (`&`)
* no dereference (`*`)

*Compiler may use internal addressing. Null value = 0.*

---

## 16. Error Handling

* No exceptions.
* Runtime traps: divide by zero, invalid instruction, illegal memory access.
* Undefined behavior: stack overflow, missing return.

---

## 17. Comments

```c
// single line
/* multi line */
```

---

## 18. Reserved Keywords

```
int, void, if, else, while, return, asm, const
```

---

## 19. Compiler Contract

### 19.1 Required Stages

1. Lexer
2. Parser (AST)
3. Semantic Analysis (Basic check)
4. CodeGen (HelixASM emission)

### 19.2 Determinism
Compiler must produce identical HelixASM output for identical source input.

---

## 20. Versioning

This spec is **Helix C-lite v0.1 (Frozen)**.
Backward compatibility: v0.1 programs must compile in v0.2+.

---
