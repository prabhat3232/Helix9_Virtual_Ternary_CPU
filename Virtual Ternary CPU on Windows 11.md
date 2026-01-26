# **Feasibility and Architecture of the Helix-9: An Advanced Virtual Balanced Ternary Processor on Windows 11**

## **Executive Summary: The Renaissance of Ternary Computing**

The architecture of modern computing is founded almost exclusively on binary logic, a decision driven by the reliability of early vacuum tubes and the simplicity of Boolean algebra. However, the theoretical landscape of computer science has long harbored an alternative: ternary computing. Utilizing base-3 logic, specifically the balanced ternary system (comprising states ![][image1]), offers mathematical elegance and information density superior to binary systems. While hardware constraints stifled the development of ternary silicon in the 20th century, the immense computational power of modern binary processors, such as those running Microsoft Windows 11, presents a new frontier. It is not only possible but architecturally advantageous to create an advanced virtual ternary CPU—a "Helix-9" processor—atop the Windows 11 subsystem.

This report confirms the feasibility of such an undertaking and provides an exhaustive architectural specification. The proposed system does not merely simulate ternary logic for academic curiosity but emulates a high-performance RISC architecture capable of executing complex operating systems and applications. By leveraging the advanced APIs of Windows 11, including DirectX 12 for GPU-accelerated ternary framebuffers, multi-threaded execution schedulers, and Just-In-Time (JIT) binary translation, the Helix-9 can bridge the gap between binary hardware and ternary software.

The analysis reveals that the primary challenge is the "emulation penalty" incurred by mapping trits (ternary digits) to bits. However, utilizing a novel **2-bit Biased Encoding scheme**, combined with 64-bit parallel Arithmetic Logic Unit (ALU) operations, allows the emulator to achieve effective clock speeds sufficient for real-time applications. Furthermore, the report details the implementation of the **Ternary27** floating-point standard, which offers a dynamic range vastly superior to the IEEE 754 single-precision binary standard, making the virtual machine a potent tool for numerical analysis.

The following sections dismantle the binary hegemony, reconstructing a complete computing stack from the mathematical axioms of Kleene logic up to the high-level integration with the Windows Desktop Window Manager (DWM). This is a blueprint for the post-binary era, simulated within the binary present.

## **1\. The Theoretical Imperative: Why Ternary?**

To understand the motivation behind building a complex virtual machine for ternary logic, one must first appreciate the fundamental inefficiencies of binary computing and the elegant solutions offered by the ternary domain. The quest for the "Helix-9" is rooted in the mathematical search for the optimal radix.

### **1.1 The Radix Economy and Hardware Efficiency**

The efficiency of a number system in a computer is governed by its **radix economy**, defined as the product of the radix (base) ![][image2] and the width (number of digits) ![][image3] required to represent a given value ![][image4]. The cost function is ![][image5]. This represents the rough hardware cost: the number of distinct states a gate must support multiplied by the number of gates needed to store the number.

Calculus reveals that the most efficient radix is the mathematical constant ![][image6] (![][image7]).

* **Binary (![][image8]):** The efficiency ratio is approximately ![][image9] times the optimal cost (![][image10]).  
* **Ternary (![][image11]):** The efficiency ratio is approximately ![][image12] times the optimal cost (![][image13]).  
* **Decimal (![][image14]):** The efficiency is significantly worse, requiring far more hardware states than necessary for the information carried.

Ternary is the integer radix closest to ![][image6], making it theoretically the most efficient integer base for computing. A ternary computer requires fewer trits to represent a number than a binary computer requires bits, and remarkably, the product of "states ![][image15] count" is lower for ternary. For instance, to represent the value 100,000:

* **Binary:** Requires 17 bits (![][image16]). Cost: ![][image17].  
* **Ternary:** Requires 11 trits (![][image18]). Cost: ![][image19].

While a cost difference of 1 seems marginal, extrapolated over billions of transistors and logic gates, the savings in complexity and wiring density are substantial. In a virtual environment, this translates to information density: a single 27-trit word carries the information equivalent to 42.79 bits, allowing a 64-bit integer container in the host machine to store vastly more range than a standard 32-bit integer, provided the encoding is efficient.

### **1.2 The Legacy of Setun and Balanced Ternary**

The history of computing is littered with binary machines, but the Soviet **Setun**, developed at Moscow State University in 1958 by Nikolay Brusentsov, stands as the only mass-produced ternary computer. Setun did not use "unbalanced" ternary (0, 1, 2\) but rather **Balanced Ternary** (![][image1]).

This distinction is critical for the design of the Helix-9. Unbalanced ternary suffers from the same arithmetic awkwardness as unsigned binary; it requires complex rules or separate sign bits to handle negative numbers. Balanced ternary, however, is a signed-digit representation where the sign is inherent in the digits themselves.

**Advantages of Balanced Ternary for the Virtual CPU:**

1. **Unified Arithmetic:** There is no distinction between addition and subtraction. Subtraction is simply the addition of the inverse. To subtract ![][image20] from ![][image21], one inverts the sign of every trit in ![][image20] (a trivial operation) and adds it to ![][image21].  
2. **Sign Detection:** The sign of a number is determined solely by the most significant non-zero trit. There is no dedicated "sign bit" that can be lost or corrupted.  
3. **Rounding:** Truncation in balanced ternary is equivalent to rounding to the nearest integer. This minimizes the accumulation of rounding errors in complex calculations, a feature that made Setun particularly effective for numerical analysis despite its modest clock speed.

The Setun-70, a later iteration, introduced the concept of a stack-based architecture optimized for structured programming (specifically the DSSP language), influencing the design of the Helix-9 to adopt a hybrid RISC-stack approach to facilitate compiler development.1

### **1.3 Information Density and Entropy**

In information theory, a single trit carries ![][image22] bits of information. A standard byte (8 bits) can store 256 values. A "Tryte" (typically 6 or 9 trits) operates differently.

* **6-trit Tryte:** ![][image23] values (comparable to 9.5 bits).  
* **9-trit Tryte:** ![][image24] values (comparable to 14.3 bits).

For the Helix-9 architecture running on Windows 11, aligning with the host's 8-bit byte and 64-bit word structures is challenging. A 9-trit tryte fits comfortably within a 16-bit binary word (uint16\_t), leaving some bits unused. However, this slight "waste" of host memory space is a worthy tradeoff for the computational elegance gained in the virtual environment. The higher information density per "register" in the virtual machine means fewer instructions are needed to fetch large constants or address vast memory spaces, improving the effective instruction throughput of the emulator.

## **2\. Mathematical and Logical Foundations**

Before a single line of C++ or Rust code can be written for the Windows-based emulator, the mathematical laws governing the virtual universe must be codified. The Helix-9 operates on **Kleene Logic** and utilizes **Heptavintimal** notation.

### **2.1 Ternary Boolean Algebra (Kleene Logic)**

Standard Boolean algebra (True/False) is insufficient. We employ Kleene's strong logic of indeterminacy, mapping:

* **True (![][image25]):** Value ![][image26]  
* **Unknown (![][image27]):** Value ![][image28]  
* **False (![][image29]):** Value ![][image30]

The fundamental logic gates required for the Helix-9 ALU are:

**2.1.1 Minimum (AND Equivalent)**

The ternary AND operation is defined as the minimum of the two inputs.

![][image31]

| A | B | Result |
| :---- | :---- | :---- |
| \+ | \+ | \+ |
| \+ | 0 | 0 |
| \+ | \- | \- |
| 0 | 0 | 0 |
| 0 | \- | \- |
| \- | \- | \- |

**2.1.2 Maximum (OR Equivalent)**

The ternary OR operation is defined as the maximum of the two inputs.

![][image32]

| A | B | Result |
| :---- | :---- | :---- |
| \+ | \+ | \+ |
| \+ | 0 | \+ |
| \+ | \- | \+ |
| 0 | 0 | 0 |
| 0 | \- | 0 |
| \- | \- | \- |

**2.1.3 Negation (NOT Equivalent)**

Inversion is geometric reflection across zero.

![][image33]

| A | Result |
| :---- | :---- |
| \+ | \- |
| 0 | 0 |
| \- | \+ |

**2.1.4 Modulo Sum (XOR Equivalent)**

The exclusive OR in binary represents "sum without carry." In ternary, this is the modulo 3 sum.

![][image34]  
This operation is crucial for the "half-adder" logic within the ALU.

### **2.2 The 2-Bit Encoding Problem**

Emulating ternary on a binary Windows machine requires representing 3 states using binary bits. The choice of encoding dictates the efficiency of the ALU. A single bit (![][image35]) is insufficient. Two bits (![][image36]) are required, resulting in one wasted state.

There are two primary candidates for encoding:

**Option A: 2-Bit Unsigned Integer Mapping**

* **![][image37]**  
* **![][image38]**  
* **![][image39]** (Unbalanced) or mapped to ![][image30].  
* ![][image40] Illegal.  
* *Verdict:* Poor for arithmetic. Implementing balanced math requires complex lookup tables or branching logic to handle the signs.

**Option B: Balanced Sign-Magnitude (Biased Encoding)**

This is the superior method for the Helix-9. We utilize two parallel binary words to represent a vector of trits.

* **Bit P (![][image41]):** Represents the ![][image26] component.  
* **Bit N (![][image42]):** Represents the ![][image30] component.  
* **Value ![][image43].**

| State | Bit N (b1​) | Bit P (b0​) | Calculation |
| :---- | :---- | :---- | :---- |
| **\+1** | 0 | 1 | ![][image44] |
| **0** | 0 | 0 | ![][image45] |
| **\-1** | 1 | 0 | ![][image46] |
| **Illegal** | 1 | 1 | ![][image47] |

**Why this is optimal for Windows 11 (x64):**

Modern CPUs like the Intel Core or AMD Ryzen series possess powerful 64-bit ALUs. By using this encoding, we can pack 64 trits into a pair of 64-bit registers (uint64\_t P, uint64\_t N). Operations on 64 trits can happen in parallel using bitwise instructions, achieving massive throughput (SIMD-within-a-register). The "Illegal" state (![][image48]) naturally cancels out to ![][image28] in subtraction (![][image49]), meaning in some additive contexts, error correction is automatic.

### **2.3 Deriving 64-bit ALU Operations**

To prove feasibility, we must derive the Boolean formulas that the host CPU will execute to simulate ternary gates.

**Ternary MIN (AND):**

**![][image50]**  
**![][image51]**  
*Reasoning:* For the result to be ![][image26] (![][image52]), both inputs must be ![][image26]. For the result to be ![][image30] (![][image53]), if either input is ![][image30], the minimum pulls it down to ![][image30].

**Ternary MAX (OR):**

**![][image54]**  
**![][image55]**  
*Reasoning:* If either input is ![][image26], the max is ![][image26]. Only if both are ![][image30] is the result ![][image30].

**Ternary NOT (Negation):**

**![][image56]**  
**![][image57]**  
*Reasoning:* Simply swapping the Positive and Negative channels inverts the value. This is a single CPU cycle operation (XCHG or simplistic pointer swap).

**Ternary Half-Adder (Sum and Carry):**

The most complex derivation. We need outputs Sum (![][image58]) and Carry (![][image59]) from inputs ![][image21] and ![][image20].

Logic Table for Balanced Addition:

* ![][image60] Sum ![][image30] (![][image25]), Carry ![][image26].  
* ![][image61] Sum ![][image26], Carry ![][image28].  
* ![][image62] Sum ![][image28], Carry ![][image28].  
* ![][image63] Sum ![][image28], Carry ![][image28].  
* ![][image64] Sum ![][image26], Carry ![][image30] (![][image25]).

**Host Bitwise Implementation:**

Let inputs be ![][image65] and ![][image66].

1. **Generate Carries:**  
   Overflow occurs when adding two likes.  
   ![][image67]  
   (Two ![][image26]s generate a ![][image26] carry)  
   ![][image68]  
   (Two ![][image30]s generate a ![][image30] carry)  
2. **Calculate Raw Sum (ignoring carry/cancel):**  
   ![][image69]  
   ![][image70]  
3. **Handle Cancellation (![][image71]):**  
   If we have a ![][image26] and a ![][image30], they annihilate.  
   ![][image72]  
4. **Final Sum:**  
   The sum logic must account for the "wrap around" effect of the carry.  
   If ![][image73], ![][image74] is 0, but Sum should be ![][image30]. So ![][image75] contributes to ![][image76].  
   If ![][image77], ![][image78] is 0, but Sum should be ![][image26]. So ![][image79] contributes to ![][image80].  
   ![][image81]  
   ![][image82]

This derivation proves that a full balanced ternary adder can be implemented using standard binary logic gates (AND, OR, XOR, NOT), allowing the Helix-9 to perform arithmetic at native hardware speeds.

## **3\. Historical Context and Modern Architectures**

The design of the Helix-9 is not created in a vacuum but is a synthesis of historical innovation and modern experimental architectures.

### **3.1 The Soviet Setun and Setun-70**

The **Setun** (1958) was a magnetic logic computer. Its architecture was simpler than contemporary binary machines due to the efficiency of ternary. It used a 1-address architecture with a single accumulator.

* **Word Size:** 18 trits (approx 29 bits).  
* **Memory:** 81 words of magnetic core RAM (later expanded).  
* **Legacy:** Setun demonstrated that ternary logic could reduce the component count (tubes/transistors) by roughly 40% compared to a binary computer of equal power.

The **Setun-70** (1970) was more advanced, introducing a stack-based architecture (Dual Stack: Return Stack and Operand Stack) which directly influenced the design of the Forth programming language. The Helix-9 adopts this "Stack \+ Register" hybrid approach. While pure stack machines are efficient for compilers, register machines are faster for pipelined execution. Helix-9 uses registers for hot data and a stack for procedure calls, mimicking the efficiency of Setun-70’s structured programming model (DSSP).1

### **3.2 The ART-9 RISC Architecture**

Recent academic work has produced the **ART-9** (Advanced RISC-based Ternary) processor, a 9-trit architecture designed for FPGA implementation.

* **Significance:** ART-9 proves that standard RISC concepts (Fetch-Decode-Execute pipelines, Load/Store architecture) apply seamlessly to ternary.  
* **Benchmarks:** ART-9 achieved high Dhrystone scores per watt in simulation, validating the low-power potential of ternary logic.4

### **3.3 The Termite and Triador Virtual Machines**

Modern hobbyist emulators provide practical implementation details.

* **Termite:** A virtual machine using 9-trit trytes and a RISC-like instruction set. It uses a textual assembly language and runs on Windows/Linux.6  
* **Triador:** A visual emulator for a simplistic ternary CPU, useful for understanding the visualization of ternary states (often using Red/Green/Blue LEDs for \-, 0, \+).8

The Helix-9 supersedes these by targeting "Advanced" capabilities: 64-bit host optimizations, floating-point support, and OS-level integration, rather than just simple microcontroller simulation.

## **4\. Proposed Architecture: The Helix-9 Virtual CPU**

The Helix-9 is designed as a generic, high-performance virtual processor.

### **4.1 Specification Overview**

* **Word Size:** 27 Trits. This is the "Golden Word."  
  * ![][image83] (7.6 Trillion states).  
  * Fits into a 64-bit integer (using 54 bits for 2-bit encoding).  
  * Sufficient address space for Terabytes of RAM.  
* **Endianness:** Big-Endian (Most Significant Trit first), honoring the mathematical notation of balanced ternary.  
* **Registers:**  
  * **R0 \- R15:** 16 General Purpose Registers (27-trit).  
  * **PC:** Program Counter.  
  * **SP:** Stack Pointer.  
  * **ST:** Status Register (Flags: Zero, Negative, Overflow).

### **4.2 Pipeline Design**

To achieve high throughput on the Windows host, the emulator implements a classic 5-stage pipeline.

1. **IF (Instruction Fetch):** Retrieve instruction from memory at PC.  
2. **ID (Instruction Decode):** Extract opcode and operands.  
3. **EX (Execute):** Perform ALU operation or address calculation.  
4. **MEM (Memory Access):** Read/Write to data RAM (if Load/Store).  
5. **WB (Write Back):** Update destination register.

**Windows Optimization:** The pipeline stages are simulated in a tight C++ loop. However, to utilize the host's branch prediction, the "Decode" phase utilizes a **computed goto** or a dense switch-case block. The emulator pre-decodes instructions into an internal "Micro-Op" format stored in a "JIT Cache" to avoid re-decoding the same ternary opcodes repeatedly.10

### **4.3 Memory Management Unit (MMU)**

The Helix-9 does not simply access an array. It implements a virtual paging system to handle the massive address space (![][image84] addresses).

* **Page Size:** ![][image85] Trytes (19,683 words).  
* **Page Table:** A hash map (std::unordered\_map in C++) mapping high-order trits of the address to allocated binary memory blocks.  
* **Windows Integration:** The emulator uses VirtualAlloc on Windows to reserve large chunks of host RAM only when the ternary OS requests them, preventing the emulator from consuming 100GB of RAM at startup.

### **4.4 The Instruction Set Architecture (ISA)**

The ISA is RISC-V inspired but adapted for ternary. Instructions are fixed length (18 trits or 2 Trytes).

| Opcode | Mnemonic | Operands | Function |
| :---- | :---- | :---- | :---- |
| 001 | LDT | Rd, | Load Tryte from Memory |
| 002 | STT | Rs, | Store Tryte to Memory |
| 004 | ADD | Rd, Rs1, Rs2 | Integer Addition |
| 005 | SUB | Rd, Rs1, Rs2 | Integer Subtraction |
| 006 | MUL | Rd, Rs1, Rs2 | Integer Multiplication |
| 010 | MIN | Rd, Rs1, Rs2 | Kleene Logic AND |
| 011 | MAX | Rd, Rs1, Rs2 | Kleene Logic OR |
| 012 | NEG | Rd, Rs1 | Logic Invert (Sign flip) |
| 020 | JMP | Offset | Unconditional Jump |
| 021 | JEZ | Rs, Offset | Jump if Zero |
| 022 | JLZ | Rs, Offset | Jump if Less Than Zero (-1) |
| 023 | JGZ | Rs, Offset | Jump if Greater Than Zero (+1) |

**Insight:** The branching efficiency is higher than binary. A single comparison instruction (CMP) sets the state to ![][image86]. A subsequent branch can target any of these states directly. The JLZ and JGZ instructions are incredibly fast because they only need to check the "Sign Trit" of the value.

## **5\. Advanced Arithmetic: Integers and Floating Point**

### **5.1 Integer Multiplication**

Binary multiplication is shift-and-add. Ternary multiplication is similar but with three cases for the multiplier digit:

* If digit is ![][image87]: Add Multiplicand.  
* If digit is ![][image28]: Do nothing.  
* If digit is ![][image30]: **Subtract** Multiplicand.

This "Subtract" capability is unique. In binary, multiplication by numbers with many 1s is slow (many adds). In balanced ternary, a number like ![][image88] is ![][image89] (![][image90]). This contains only two non-zero trits, requiring only one addition and one subtraction, whereas binary ![][image91] requires three additions. This suggests that for sparse-trit numbers, ternary multiplication is computationally cheaper.12

### **5.2 The Ternary27 Floating Point Standard**

To be an "Advanced" CPU, Helix-9 implements the **Ternary27** standard, a proposed specification for balanced ternary floating-point arithmetic that rivals IEEE 754\.

**Format Layout (27 Trits):**

* **Significand (Mantissa):** 20 trits.  
  * Includes the sign. Unlike binary IEEE 754 which has a separate sign bit and an implicit leading 1, Ternary27 uses the whole significand. The leading trit is normalized to be non-zero (![][image87] or ![][image30]).  
* **Exponent:** 7 trits.  
  * Range: ![][image92] to ![][image93].

**Comparison to IEEE 754 Single Precision (Binary32):**

| Feature | IEEE 754 (Binary32) | Ternary27 | Advantage |
| :---- | :---- | :---- | :---- |
| **Bits/Trits** | 32 Bits | 27 Trits (\~42.8 Bits) | Higher Precision |
| **Significand** | 23 bits (+1 implicit) | 20 trits (\~31.7 bits) | **8 bits more precision** |
| **Exponent Range** | **![][image94]** | **![][image95]** | **\~8.6x Larger Range** |
| **Smallest Values** | **![][image96]** | **![][image97]** | Massive dynamic range |
| **Largest Values** | **![][image98]** | **![][image99]** | Massive dynamic range |

**Implementation Challenges:**

Emulating Ternary27 on Windows is non-trivial. The host CPU (x64) does not have hardware support for this format.

* **Approach:** Software Emulation Library (SoftFloat).  
* **Algorithm:** Operations (Add, Sub, Mul) break down the 27-trit word into exponent and mantissa integers. The mantissa operations use the integer ALU functions derived in Section 2.3.  
* **Rounding:** Ternary27 defines "Round to Nearest Odd" (analogous to Round to Nearest Even). Because the base is odd (![][image100]), division by 2 is not exact, but division by 3 is a simple shift. This changes the numerical stability properties of algorithms, often improving convergence for specific trigonometric functions.15

## **6\. Windows 11 Integration and Implementation**

The Helix-9 is not just a console application; it is a graphical virtual machine integrated into the Windows 11 desktop environment.

### **6.1 The Host Application Structure**

The emulator is built as a multi-threaded Win32/DirectX application.

**Thread Architecture:**

1. **Main Thread (UI):** Handles the Windows Message Pump (WndProc), window resizing, and menu commands.  
2. **Core Thread (CPU):** The infinite loop executing the Helix-9 pipeline. This thread is pinned to a specific host core (Processor Affinity) to maximize cache locality.  
3. **Render Thread (GPU):** Synced to the monitor's refresh rate (VSync), this thread reads the Ternary VRAM and updates the DirectX Swap Chain.

### **6.2 The Graphics Pipeline: DirectX 12 Integration**

Visualizing a ternary computer is a unique challenge. How do we display trits?

We define a **Ternary Framebuffer** mapped to a region of the virtual RAM.

**Specifications:**

* **Resolution:** ![][image101] (Standard Mode), ![][image102] (High Res).  
* **Pixel Format:** 1 Tryte (9 trits) per pixel.  
  * This provides ![][image24] distinct colors.

**The "Ternary DAC" Shader:**

To display this on a standard RGB monitor, we use a DirectX Compute Shader or Pixel Shader.

We interpret the 9 trits as three channels of 3 trits each:

* **Red Channel:** Trits 0-2 (Range ![][image103] to ![][image104]).  
* **Green Channel:** Trits 3-5.  
* **Blue Channel:** Trits 6-8.

**HLSL Shader Logic (Pseudocode):**

High-level shader language

// Converts a balanced ternary integer (-13 to \+13) to normalized float (0.0 to 1.0)  
float TernaryToFloat(int ternaryValue) {  
    // Map \-13..+13 to 0..26  
    float offset \= ternaryValue \+ 13.0;  
    return offset / 26.0;  
}

float4 main(PixelInput input) : SV\_TARGET {  
    // Fetch 9-trit value from structured buffer  
    int rawTrits \= TernaryVRAM.Load(input.uv);  
      
    int tR \= ExtractTrits(rawTrits, 0, 3);  
    int tG \= ExtractTrits(rawTrits, 3, 3);  
    int tB \= ExtractTrits(rawTrits, 6, 3);  
      
    return float4(TernaryToFloat(tR), TernaryToFloat(tG), TernaryToFloat(tB), 1.0);  
}

This shader runs on the host GPU (NVIDIA/AMD/Intel), performing the color conversion instantly. The CPU emulator only needs to write the raw trits to a ID3D12Resource buffer, ensuring high performance.16

### **6.3 Input and Audio**

* **Keyboard:** Windows WM\_KEYDOWN events are captured. Keys are mapped to a specific ternary encoding (e.g., 'A' might be mapped to a specific 6-trit value). These values are written to a ring buffer at a fixed memory address (0xF000). An interrupt is raised on the Helix-9 CPU to notify the Guest OS.  
* **Audio:** Using the Windows Audio Session API (WASAPI) in exclusive mode. The Helix-9 fills a circular buffer with balanced ternary samples. A dedicated thread converts these samples (linear mapping from range ![][image105] to ![][image106]) and pushes them to the audio driver.

## **7\. Optimization: JIT Compilation**

The "Advanced" designation of this project implies performance beyond a simple interpreter. Interpreters suffer from the overhead of the "fetch-decode-dispatch" loop. To run real software, we implement **Dynamic Binary Translation (JIT)**.

### **7.1 The Translation Engine**

The JIT compiler inspects blocks of ternary instructions and translates them into a sequence of x64 machine code instructions.

**Example Translation:**

* **Ternary:** ADD R1, R2, R3 (Add R2 and R3, store in R1).  
* **x64 JIT Output:**  
  1. Load R2.pos and R2.neg into CPU registers rax, rbx.  
  2. Load R3.pos and R3.neg into rcx, rdx.  
  3. Execute the bitwise logic derived in Section 2.3 (approx 10-15 x64 instructions).  
  4. Store results back to R1 structure in memory.

### **7.2 SIMD Vectorization**

Windows 11 runs on processors supporting AVX2 and AVX-512. A 27-trit word uses only 54 bits of a 64-bit container. However, with AVX-512, we can process **8 ternary words simultaneously** (512 bits / 64 bits \= 8). The JIT compiler can detect loops performing vector math (e.g., image processing in the guest OS) and emit AVX-512 instructions. The bitwise logic for ternary gates (AND, OR, XOR) maps 1:1 to AVX logic instructions (vpand, vpor, vpxor). This allows the Helix-9 to achieve "Super-Ternary" speeds, potentially outperforming sequential C++ execution by an order of magnitude.19

## **8\. Conclusion**

The creation of the Helix-9 Virtual CPU on Windows 11 is a triumphant convergence of history and futurism. By revisiting the discarded path of balanced ternary logic, we uncover a computing model of startling efficiency and mathematical beauty.

Through the rigorous application of 2-bit biased encoding, we have demonstrated that the "alien" logic of ternary can be efficiently hosted on binary silicon. The derivation of the boolean algebra for the ALU proves that no exotic hardware is strictly necessary—only clever software. The integration with Windows 11 via DirectX and multi-threading transforms this from a theoretical exercise into a usable workstation.

The Helix-9 project satisfies the original inquiry not just by confirming possibility, but by providing the blueprint for a machine that challenges the binary assumptions of the digital age. It serves as a reminder that the evolution of computing was not a straight line, and in the infinite flexibility of virtualization, the "roads not taken" are still open for exploration.

### ---

**List of Tables**

* **Table 1:** Boolean Logic for Ternary Min/Max.  
* **Table 2:** Balanced Ternary Addition Logic.  
* **Table 3:** Biased Encoding Bit Mapping.  
* **Table 4:** Helix-9 ISA Opcodes.  
* **Table 5:** Ternary27 vs IEEE 754 Comparison.

### **Citations**

* **Radix Economy:** 20  
* **Setun/History:** 1  
* **Ternary Logic/ALU:** 14  
* **Floating Point:** 15  
* **Architecture/ISA:** 4  
* **Windows Integration:** 16

#### **Works cited**

1. Setun \- Wikipedia, accessed January 25, 2026, [https://en.wikipedia.org/wiki/Setun](https://en.wikipedia.org/wiki/Setun)  
2. Setun, accessed January 25, 2026, [http://www.staroceans.org/wiki/A/Setun](http://www.staroceans.org/wiki/A/Setun)  
3. How does Setun and Setun 70 work? I know it uses a ternary system and not binary. \- Quora, accessed January 25, 2026, [https://www.quora.com/How-does-Setun-and-Setun-70-work-I-know-it-uses-a-ternary-system-and-not-binary](https://www.quora.com/How-does-Setun-and-Setun-70-work-I-know-it-uses-a-ternary-system-and-not-binary)  
4. Design and Evaluation Frameworks for Advanced RISC-based Ternary Processor \- DATE 2019, accessed January 25, 2026, [https://past.date-conference.com/proceedings-archive/2022/pdf/0113.pdf](https://past.date-conference.com/proceedings-archive/2022/pdf/0113.pdf)  
5. Design and Evaluation Frameworks for Advanced RISC-based Ternary Processor | Request PDF \- ResearchGate, accessed January 25, 2026, [https://www.researchgate.net/publication/360732442\_Design\_and\_Evaluation\_Frameworks\_for\_Advanced\_RISC-based\_Ternary\_Processor](https://www.researchgate.net/publication/360732442_Design_and_Evaluation_Frameworks_for_Advanced_RISC-based_Ternary_Processor)  
6. ternary-computer · GitHub Topics, accessed January 25, 2026, [https://github.com/topics/ternary-computer](https://github.com/topics/ternary-computer)  
7. Termite is a virtual machine for a ternary-based RISC CPU architecture. \- GitHub, accessed January 25, 2026, [https://github.com/nirvanasupermind/termite](https://github.com/nirvanasupermind/termite)  
8. ssloy/triador: Ternary computer emulator \- GitHub, accessed January 25, 2026, [https://github.com/ssloy/triador](https://github.com/ssloy/triador)  
9. Ternary computer program emulator | Details \- Hackaday.io, accessed January 25, 2026, [https://hackaday.io/project/28579/log/175489-ternary-computer-program-emulator](https://hackaday.io/project/28579/log/175489-ternary-computer-program-emulator)  
10. ECE 4750 Computer Architecture, Fall 2025 Lab 2: Pipelined Processor, accessed January 25, 2026, [https://www.csl.cornell.edu/courses/ece4750/handouts/ece4750-lab2-proc.pdf](https://www.csl.cornell.edu/courses/ece4750/handouts/ece4750-lab2-proc.pdf)  
11. Principles of Pipelining \- Computer Science and Engineering, accessed January 25, 2026, [https://www.cse.iitd.ac.in/\~srsarangi/archbook/chapters/pipelining.pdf](https://www.cse.iitd.ac.in/~srsarangi/archbook/chapters/pipelining.pdf)  
12. Balanced ternary \- Wikipedia, accessed January 25, 2026, [https://en.wikipedia.org/wiki/Balanced\_ternary](https://en.wikipedia.org/wiki/Balanced_ternary)  
13. Fast Ternary Multiplication and Division, accessed January 25, 2026, [https://homepage.cs.uiowa.edu/\~jones/ternary/multiply.shtml](https://homepage.cs.uiowa.edu/~jones/ternary/multiply.shtml)  
14. Efficient Ternary Logic Circuits Optimized by Ternary Arithmetic Algorithms \- Hajim School of Engineering & Applied Sciences, accessed January 25, 2026, [https://hajim.rochester.edu/ece/sites/friedman/papers/TEmerging\_24.pdf](https://hajim.rochester.edu/ece/sites/friedman/papers/TEmerging_24.pdf)  
15. Ternary27 A Balanced Ternary Floating Point Format Version 3.1 Introduction \- Hackaday.io, accessed January 25, 2026, [https://cdn.hackaday.io/files/1649077055381088/Ternary27%20Standard.pdf](https://cdn.hackaday.io/files/1649077055381088/Ternary27%20Standard.pdf)  
16. Windows with C++: Layered Windows with Direct2D | Microsoft Learn, accessed January 25, 2026, [https://learn.microsoft.com/en-us/archive/msdn-magazine/2009/december/windows-with-c-layered-windows-with-direct2d](https://learn.microsoft.com/en-us/archive/msdn-magazine/2009/december/windows-with-c-layered-windows-with-direct2d)  
17. Understand the Direct3D 11 rendering pipeline \- Win32 apps \- Microsoft Learn, accessed January 25, 2026, [https://learn.microsoft.com/en-us/windows/win32/direct3dgetstarted/understand-the-directx-11-2-graphics-pipeline](https://learn.microsoft.com/en-us/windows/win32/direct3dgetstarted/understand-the-directx-11-2-graphics-pipeline)  
18. Writing directly to screen/framebuffer skipping any pipelines? : r/GraphicsProgramming, accessed January 25, 2026, [https://www.reddit.com/r/GraphicsProgramming/comments/19dmpbr/writing\_directly\_to\_screenframebuffer\_skipping/](https://www.reddit.com/r/GraphicsProgramming/comments/19dmpbr/writing_directly_to_screenframebuffer_skipping/)  
19. AVX Bitwise ternary logic instruction busted\!, accessed January 25, 2026, [https://arnaud-carre.github.io/2024-10-06-vpternlogd/](https://arnaud-carre.github.io/2024-10-06-vpternlogd/)  
20. Douglas W. Jones on Ternary Arithmetic, accessed January 25, 2026, [https://homepage.cs.uiowa.edu/\~jones/ternary/arith.shtml](https://homepage.cs.uiowa.edu/~jones/ternary/arith.shtml)  
21. Ternary ALU \- Louis Duret-Robert, accessed January 25, 2026, [https://louis-dr.github.io/ternalu3.html](https://louis-dr.github.io/ternalu3.html)  
22. The System That Could Replace Binary And Change Computers FOREVER \- YouTube, accessed January 25, 2026, [https://www.youtube.com/watch?v=R4achTEgXEw](https://www.youtube.com/watch?v=R4achTEgXEw)  
23. Why The Soviet TERNARY Computer Was Mysteriously Suppressed \- YouTube, accessed January 25, 2026, [https://www.youtube.com/watch?v=4vwOJE0Dq38](https://www.youtube.com/watch?v=4vwOJE0Dq38)  
24. Development of Ternary Computers at Moscow State University, accessed January 25, 2026, [https://www.computer-museum.ru/english/setun.htm](https://www.computer-museum.ru/english/setun.htm)  
25. Ternary logic to binary bit conversion using multiple input floating gate MOSFETS in 0.5 micron n-well CMOS technology, accessed January 25, 2026, [https://repository.lsu.edu/cgi/viewcontent.cgi?article=1085\&context=gradschool\_theses](https://repository.lsu.edu/cgi/viewcontent.cgi?article=1085&context=gradschool_theses)  
26. XXIIVV — ternary logic, accessed January 25, 2026, [https://wiki.xxiivv.com/site/ternary\_logic.html](https://wiki.xxiivv.com/site/ternary_logic.html)  
27. Schematic diagram of the Balanced Ternary Adder (C1, C2, C3, C4, C5,... \- ResearchGate, accessed January 25, 2026, [https://www.researchgate.net/figure/Schematic-diagram-of-the-Balanced-Ternary-Adder-C1-C2-C3-C4-C5-C6-C7-C8-C11\_fig1\_4238280](https://www.researchgate.net/figure/Schematic-diagram-of-the-Balanced-Ternary-Adder-C1-C2-C3-C4-C5-C6-C7-C8-C11_fig1_4238280)  
28. Douglas W. Jones on Ternary Numbers, accessed January 25, 2026, [https://homepage.divms.uiowa.edu/\~jones/ternary/numbers.shtml](https://homepage.divms.uiowa.edu/~jones/ternary/numbers.shtml)  
29. Bias value and range of the exponent of floating point \- Stack Overflow, accessed January 25, 2026, [https://stackoverflow.com/questions/65388685/bias-value-and-range-of-the-exponent-of-floating-point](https://stackoverflow.com/questions/65388685/bias-value-and-range-of-the-exponent-of-floating-point)  
30. brandonssmith/ternary: A ternary computer simulator. \- GitHub, accessed January 25, 2026, [https://github.com/brandonssmith/ternary](https://github.com/brandonssmith/ternary)  
31. 16 bit risc instruction set \- Google Groups, accessed January 25, 2026, [https://groups.google.com/g/retro-comp/c/t3M8HqNeb6M](https://groups.google.com/g/retro-comp/c/t3M8HqNeb6M)  
32. How can I port C++ code that uses the ternary operator to Rust? \- Stack Overflow, accessed January 25, 2026, [https://stackoverflow.com/questions/45092638/how-can-i-port-c-code-that-uses-the-ternary-operator-to-rust](https://stackoverflow.com/questions/45092638/how-can-i-port-c-code-that-uses-the-ternary-operator-to-rust)  
33. Writing a Compiler and a Virtual Machine in Rust \- The Tech Trailblazer, accessed January 25, 2026, [https://thetechtrailblazer.blog/2023/03/20/writing-a-compiler-and-a-virtual-machine-in-rust/](https://thetechtrailblazer.blog/2023/03/20/writing-a-compiler-and-a-virtual-machine-in-rust/)  
34. Framebuffer | TouchGFX Documentation, accessed January 25, 2026, [https://support.touchgfx.com/docs/basic-concepts/framebuffer](https://support.touchgfx.com/docs/basic-concepts/framebuffer)  
35. Performance, Methods, and Practices of DirectX\* 11 Multithreaded Rendering \- Intel, accessed January 25, 2026, [https://www.intel.com/content/dam/develop/external/us/en/documents/performance-methods-and-practices-of-directx-11-multithreaded-rendering.pdf](https://www.intel.com/content/dam/develop/external/us/en/documents/performance-methods-and-practices-of-directx-11-multithreaded-rendering.pdf)

[image1]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEwAAAAXCAYAAACh3qkfAAACL0lEQVR4Xu2YP0jdQBzHf6KCLrY4tIiC1qW4WVoqSEeVKoggRQdBBJF2a3GoIDiJg9CluomITm3FWcRVB8EuroKgIC0O6qIgFP98v17CM5ecL5fk1fjIBz74vHsX7765u1wUycjIyMhweQU79EILquBnOAcnYI23+r9RCYfgU608EVrgV7gNr+GYtzo09XAHjsAK2Al34du7Xyog1bAfLsJTeCAFumEMrBt2wXOJFlgZnIcrzmeXKbgm6o7H4YuoG3AfDKwHvoE/pYCBubyW6IE1wr/ib9sr6pq8dhx4Xd7UsCxJygNrg1fib8tBcpkPaOW2FF1gbjB6W1O5LUUXGNsEBZPawMrhc1FfyOczWKqaeYgT2KgEB2MbmGkck3AwoNw0lryBNYs6+4TxO2y4beUlTmCmYEzlJlrF31/6G64HlJvGkjewJIgT2Dv4T/xt3cD4tIxD4ksyCfIFxlOz6eRcC/fhjFb+ER7DJud3Lh8uIx5sbUh1YON6BXgJj+AhfKHVkRJR+8wWfOKUcT9ahj8kd5gdFjXj7paFIUpg7GudXpEEn+AfUQNxPYEbomYD4R/maw/PWqaOM6hV+EvUqXwBboq302x7AffE7u6HCYx9ZZ/PJDeOS1HBcYwPAt8T73tF4ZLjTP3g/Ax6gnE5zop6GoYlTGCpg0vomwQvSRvYntexWZJ8err74KPhPZwWtV9FhTOOYbXrFcUGZ0Of5Db0qHDf4n8U4oSekZFibgC3KYT5AZYQwgAAAABJRU5ErkJggg==>

[image2]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAkAAAAZCAYAAADjRwSLAAAAk0lEQVR4XmNgGAWkAD4g9gRiWSgfRPsAsSRMAQ8QzwTiRiB+AsSTgHg6EJcD8TUgVgQp8gDidCDWB+JPQDwfiAWBeA8QvwViTZCiTKiCICD+DcQ2QMzIALEOZACITZwiGAC55SoQiyALIgNeID4MxEsZ0HQiA5DjQI4EeQAnQHYPTtAAxBeBWBhNHAVwMEACdQQCAD+RFscBahiuAAAAAElFTkSuQmCC>

[image3]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA4AAAAZCAYAAAABmx/yAAABA0lEQVR4Xu3RIUtDURjG8UecTRFREMGiWEwGMSw4/AQG0/wAw2RVFMPK2gQx2gwrum6wiBssLO8DCKJJbGb/j+c97MpMYpI98OPynnvOe849VxrnP2USm6hgqjA+gZl4OmvYyLUnXuAUjziPSc4u3pUmL2CAV6z65Q7OMIsOWhrucBmTvchjR3hRLKxhHWV8oJrWaA59fW+0jDulRr9fmFPHM1aidrM3HOQJMdbUsJGm8YA2SjG2p3SCragdn2a/UGsJT0o3m3McY37nuKF3yyf6Sr7qRtT+RTcqXD3ZxokKx8zx0fxNt7jHIbro4RpXSr/sx3inRaVvdtx9PozsNM5f5xP1lCnPj9sJjQAAAABJRU5ErkJggg==>

[image4]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABIAAAAYCAYAAAD3Va0xAAABGUlEQVR4Xu2TsWoCQRCGR7BRQRECYmkpCBZiIdgkWATsbJPeRhDyBNfaaKGVVlY2Yi15Ap/AvICdiI1NLEz+310ve3vkvKu9Dz5YdpbZmbk9kZioPMM9/NGuYcqIZ+GnEacrmDHOuCTgFJ7hN2x4w1c6cCneS3zk4Rz2Rd04EZXc5AO+WXs+qnAEi/AL7mDJiCfhTJ8LhDd19doRVVXPjYo8iaqYlQcyhDW9rsAj3MCc3mvCsV7/y20+vJWwjQW8wFe9x2pDz8ccLhMwERPyK0Wezw22xNbY4ouEmA+rYO91OwDeRQ19CwdWzIc9H5OCqKfAZHfnw7L53NN2QOPAAyxb+y4teJK/f4e/RdtzQsGnwH8vcD4xD8kvcTMzNIxbkGYAAAAASUVORK5CYII=>

[image5]: <data:image/png;base64,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>

[image6]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAkAAAAYCAYAAAAoG9cuAAAAoklEQVR4XmNgGAXkAnEg9gViOyBmRZNjUAPiE0C8HIgjgLgaiFcDMSdMgT4QvwbiSiBmhIqJAvFWBojJYJWbgfgJECtCFYAkpgJxKQNUE8iUT0D8C4gfMUCs7AJiY5gCEHAB4n9AXA4TwAZMgfgbA8RH6IALiJlBDH4GiBWtKNIQzSCfCsMEDID4AgPEy7MYIJr6gZgbpgAGQMaKQTHYipEBAERmFwESThcqAAAAAElFTkSuQmCC>

[image7]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEYAAAAXCAYAAAC2/DnWAAAC2UlEQVR4Xu2XXahNQRiGX6GIIuQv8pOfJClERNyQGxRKIZ3ccOHITxFRx4USUSRJypFIuOCC/N25EW4UkbggJQklFCXe17fGmv2dNfucHfam1lNP55xZs+fM+uabmW8DJSUlJfVhEN1Jj9FddFTl4yTNdA8dDRsjtj/tnHf9SXfaRHu79sBYuhc2j5W0R+Xj+jKV3qCz6ER6mX6nm2mnqF8RJ2F9i3xI+9E+dBltpe/pc1jgPEvoOTqODqHr6V06LO5UL7SCF+lq5Kvbl96hn+jkrK2InrAgnoatcLCVvoK9qFBgFtEp9CyKAzOAXoUFJKBFOUxbora6oQlqoh9g2RLYDlv1TVGbR589Qru5dmXHPhRnmzKsKDBagMewLRmzlR53bXWhKz1Ir6FyspqQAqOfKZQJk1zbeHqG9nLtgVRgRsKy7CmdnbVpjOt0YejUaLrQC/QbnVP5qCraWufpNP8gIhUYZdcW5OfTCXoJHTvn6oZeTOeLzgtlVEfRLaJsUWBTpAIjdMbtRh6cN3Qu/pHAKH1v0lOo7arU527TNf6BIxUYvfw62NaZAcsWBUdZuyrq1xCUHUfpAdhtVQvzYVk20z9wpAKjzHhAh2Z/K3uWw8Z8BLu1GkIIyjbk17bqiXm/elTnENq/3kUqMLp5NIZHAX+H9sf9KyiNdchtzH4PaFssjv7WdhmMtnteW07bT7eKbpdqpAKjdp0vHtU192CLVIQySXVXCi24CsRqZ2XhGHrJJvqZvqQvIrVSYWvog/fpFzo9awuoulWVW/TCHgVA/ycu5ISuZI0xPGoLc0sd6Pr68Bo23gj3LLAfdla1uPZAcoxQ4PlyXsYZoKv4Cn2CtiW6Iv4M6cDoO9Mt+hH52DpUNZm1WR9tX13Xb+kOugJ29au+Gpj18Si4Wixla6pu2kC/Iq/CPWEMHfp6xz/OBFjl7LdZrSgzF9CldAx+f7ySkpKSkv+FHx9GlQlLgczJAAAAAElFTkSuQmCC>

[image8]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAC0AAAAYCAYAAABurXSEAAABeklEQVR4Xu2VvSuFURjAH6HIVz5KBoksJoNioUyilJLFJouMGEQp/gHF4mMxyGawGMRmk8XATsrEoFgkfs/7vOfe974+8i73fdP51a/33nOee++5z3nOc0Q8Hk8xaMEV3MU17Cyczh69eIoD2I3H+IELWBKJywyVeITTWBqONeIFvmBPOJYptCxu8Vksy45lsWzPR8akFkewNXyvz1GxLykm5biBJ1L424tii9ZnQDXuiBX8PW7iVhhwg+0uMCXK8BDfcdANDuOM2HbotuxhPZ7hI3a5wG+YxLsEXkryTtAnVs/aSXQnAmbFFjyOb9gvdkq1PPQPpXli68SSt49VsbkALYtrbIpPpIRmdRvXxbrKF2rwHA8kWWYrxA7MX22WyBb/glvwkuRbn5bpUC4iHND61dpOQhtOJHAMG4JP/owmTS+SufC1Q9emJZwjWs9pooucwlexThY9xE8SW98qXondPmmiJaSXi/bkuA/YkQ+12tR+7fF4PP+YT7bSSs/pcoxbAAAAAElFTkSuQmCC>

[image9]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACIAAAAXCAYAAABu8J3cAAAB5ElEQVR4Xu2VTyhlYRjGH6GIWCjlzzSyk6wUkdQVahYmyUJZS83C1FhYKSXZykKRyEIiZUWzmDRNFhY2ZMWGmpqVjcxCMuZ5er/v3nPuubd7T2zUferXPd+fzvuc732/9wIFFRRfTWQsfTKHqshXskZmSV14Oaki0kmWyQrpdXNJtZAv5Jg8k63gYg59JOdkgpSRT+SKdAQ3URVkkxzA4rWSMzIQ3KSFYdJNfiN/IyVkney7Z68F8p2Uu7G+eon8ItVu7ht5ITNuHJKO9Bb5G2kmfxB92Qj5S9rdWL8aTyZ3AI0ww3pHRHGN9JN/iBoZgn3tuBvPkSfSQyphcZTGrIprxAfMZkTzCngIO5FFsgpLyzWsLkPF6hXXiALlMqIT+OnGqidfSwlyTz67cUhxjWQruExGlEKl0svH0mlF0hTXSD6p0bX9gXDxSj6WiPSduEZUfCrCbEZ0eySl5E2NqAfUI1VgDeQG1imD0jW9g/UnSZ36EWbcK6/UbCNazTWwDqoXdrk57Zknp0g1qlKyR3aQKsxaconwySWQoVhVROqoau86UvFALkib26OiO4K1b7V1LxnQ/C6svW+QE1jDCqoPFkNXeMo9TyP6wa9SMSz/o+5X40xS4Q7C/k4+pK0VVND70H+7DHDtL9WsHgAAAABJRU5ErkJggg==>

[image10]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEwAAAAYCAYAAABQiBvKAAAEK0lEQVR4Xu2YW8ilYxTH/3LI5GymkSinSYnJ1DSDcpgLxAUJF6KkkUxyY44h+YZcUOQYiYQkh3AzI4eazyGJGzdK4mIkorhQXCiH/2/W+3zz7vW+z977m29f7KnvX//2t9fz7vd913+tZ631fNIiFrE/4DDzgGxscLh5aDZOKQ42j8rGSeM48zFzSV4wVpkvaGEvgeBLVQ/IJIFgj5pX54U+HG/eYz5rbjdXDC5XcZ15WzYaJ5q7zDPzwphYaf5q/mfOKoRbKAjq9QofHzcvNg8cuCKCs9Ncm+wDYPED8wLzbHOH4kU3aXhkDzKfMM9Idn5DpGaSfb4g4m9rMoKR5W+Zt5inmfea/5jvNmttXGa+r8ozUf0dc732qo3KX5h/mqsbWx8Q6mmFcG2cZX7TfC4UL2oygt1uvmQe2XwnqPcrEmNbuagBAn6m2D0dsBV3m38osqvgLsXNNrZsGWzFvpvyAmTpJIr9pATjPtmfNeZf5oeKxtXGA+ab6ibDnrSnaL+nEK8Ap/vULyAznzNPSXZEQiwEHwauW2deocjoGtqCsQOWKzL3csU70HS4x4UKX2q4yvzavKRlY/ewi2bVDQj3J5GoxSOBqqjLHl83uDSH88yH1K1xJWNxog9EkoL7lXmzeaP5qerNoS3Y0ebrivf60XxQsa3IcrYQmZLr0TDcoEiKh/OCQkyeQRaOxDkK5ekmtajdrSiOGTzoF/P8vKBwmg70sQYde1IR0T70bclSLqhLBYwC/yo63zjg+bzHt+ZJaQ2MCvwcuBGRelndfV1wjMKRZXlBIdgPzWfGBoVTpe6xLckwulctM/oEo0z8psHujGOIONJBxa7Yan6nbocvKIKRhVWQTc+Yj6h/EC0gijPZ2KAmGOITCJz6SdFFnzev1fBn1QTDmXbNnY9g15ifmCcnextFsGrTK2Ldqb3jBepfOndFgOhQu6hhfagJRjZSdGfVLbDDMGnBEIvZ69jmO/cli3JHH7olEYEh9Y7m74Jb1T0mnKA47tScPlWRQbkmHaGIKgJkEKxDsrHBJAVjQH9Fg9ufpGBKaPsNan7sufAmxTxCVyA7Cn9Xt3jXjkIFJZMQO2PG/FJRAwuINAHIzynoE4yiz7u2W/4owRCGmsVxK/vIzJVBd/xePTWupB4Py/xZoXQB48ZTGj7BEwAEYHTIILKvmh8pOjAnjF3m6e2LGqxUdLDyLjhGveMEUmx/m1vM1xSjBjY++Z4bFsJn/wr7Cju2WdV30lhAPM6Onek3gSzEsXYmtcE8RaBy3ZgW4B+BnUn2eYODa99RKIPJnWG0b07bH7DC/Lz53GfQ+vuOQjVcab6h4SPDNIKSsl0xp+VGMC9Qt+7T+DfhOuoLHPc30wD+xcWxsDZIjw3msYuycQQYFzab5+aFKQUjEx1zwWItYhH7hv8BN/3WxDrln/cAAAAASUVORK5CYII=>

[image11]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAC0AAAAYCAYAAABurXSEAAABh0lEQVR4Xu2VvytFYRjHH0kpv/KjZJCURClkle6GEiUDswGTZLAyGiwWsSAp/8DdDMqGxcCCRbJJlIXE9/s+zznOKa5zl3vO8H7q0z3vj3N77nu+z7kiHo+nFNTCRbgD12AnLIvtyBi98ATmYD2cg+9wWTJc+Cb8hOM2ZuHn8Al2B5uyxgb8grM2roGn8FX0KYQwQ6Ow1cb8HIMt4Y7SUQGbYLmNe+CzaGSqbc5dbIsG/kH08WzBFXgN24ONKcDDPIT3sC+6MCIadh49H8GuaI6O5f8czYh+YVIvYIe7szBV8Ej0njs4LD8n71gQLXgSfsBB0S5lPPiD0u7YLvgID0R/TAzG4ko0T1mCh8aIsDnnowtBh3KxmJOtFG3WpDaLNtpfcG3JjO5jf7Ho/cicyy3zy2wXQxucKsIJ2ODu/J0B+GbyOoDFsmimISSa5zThq/YG7sE6m2uEZ6KvvX6bc6zCS9uQNvy/uIXrcBrm4YvNx2A2wxd3BmA9OdFIDUnhPvB4PJ6EfAMzMEsva25rFgAAAABJRU5ErkJggg==>

[image12]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACwAAAAXCAYAAABwOa1vAAAChUlEQVR4Xu2WS6hOURTHl1DklSjv7iWPDIwUEZlQJBIGQpigJIpiQimZGEoGEhkgj4kB3WsgYqCYkFcilBJixEDy+P/u2vues7/zfc499WWg869f9zv7rLPPf++9zlrXrFat/0+dYl3jYImGi93ipDgoxqW3e9RPzBXHxAmxXPRPIlwzxFHzuTaKIelt10yxQ9wUP8XZ9PZf1SEeiq1ikFgmXog5uRjM7hO3xWQxSpwzNzUwF7dGXDL3M1HsEvfN35GIgFVivnhnfTc8QJwSV8LvqCOiSwwO17PFB7GgN8JsingrlobrMebPYDSKhR4Xh3JjiThKJumrYV76XuxvGF8tvpkbRSyAefOpMkzcEWfMjRH7XEzLxSDmZlOaqqrhxeKXFQ2vEL/FBvM0uWZFw0PFLfMjH2nZ4l+KRSFmhLghVobrgqoajsZaGWY8GmtlOI6zy+Q5zwE7f1XsDfeaqqphDJUZjnOWGUZUDdInmv4kllgbDe+xcsN8TK+s3DCmdpqnAB8/u8scVK1N/khRVQ23MyXYycdiUrjPbq83/3ifmS+8oKqGKVM/rLVhqgXljrLXyjCVgopBJaCpNIqy98WyipOozDBf7XjLcmqCeGPFF20Xn83rO2JB+Ws0Wjyx7FneSf42irr8wNJnexUN04WiqSi6Ex3tu5gXxog5LO6ZLwbRuehWFyxrJlPNG1K+5S8UHy2bi9LFAjpjgPn8W8R5SxtTTz1lQhI8fqFfxSMxK8RwhNfN225HGEMYZfyieVs+Le5a2rEQ6fFabBObxVPzfwfixpCzlDVO4oB5Db8susXYENM28TJybG342+yfGsQpkd/A72aKMcw13YonXatWrX+pP842lwkLpWqtAAAAAElFTkSuQmCC>

[image13]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEwAAAAYCAYAAABQiBvKAAAES0lEQVR4Xu2YW6imUxjH/2JEzmYiMRc0TcSYmRyiMLto0MSFXCjlRiIpMYMitYekmSKMQ4mQ5BhKDknZDkncuFFyuKBJmULU3CiH/2+ed5n1rXetb3977/diT+1f/fv2t95vve/zPOtZz3reLS2xxL7AIdZ+5WDHodZB5eAiZZl1RDk4NMdaD1sHlxfMOusZLcwIAr5c7QUZEgL2kHVFeaHG4dbN1pPWVmu1JjPyKuvGctCcYH1onVpemJA11i7rX2tGEbiFsr+1yXrcesTaqAhSDovzjnV2MT7CWoVRU9ZR1vXWX9ZmjQ/aAdYO65RinDms1HQxPldw5g0NEzDmv2rdZB1jnWn9YL2r/g64xHpfY55JtP+2Lu++E7QvrV/VD0YO155QBC7nNOub7nOhPKdhAnaR9Y9G7b1PkcEkSA4B/Eyxe6o8oJh4bff9MOsT609F9rVgK9Zueof1toYp9kMFbEqRFGRssutOhd+3dt9zCOZr6ifDHkj9FYo9DmTG7xpvKEX+KevEYhxjCBbGjIPfTVmXKepGizxg2Md2wr5LFTZw6HCPC9SvRzmUCZ6TgsXc96zd1hnpRxnc/0dFLR4Lxf8F6yfFKdfiXGu7+jXuOMWDcKIGLQgl4CtFRl9jfar24ZAH7EjrFUWm7LS2Wfcqspwt9IH69agGgefZBIuaVvoABJFnnFVeSODIS4pAUQwv1t6Mq3GXojiW8KBfrPPKCwqnOYE+1qhjjypWtEZtS6athLMJWgFqFLVqHDcofMTGuxV+15ht4Uc42frZel71G3Io4AjbuISAYVAtzTEWp1LdY3uQYa+rnRm1gFEjywMJxwjiRA4qtiQ1iuSoHWwpYFeXF2qQomxLDMDJElZxuhzsaAWMwLNluCeLwSn6tHWl6k1vohUwnMGpxFwDBmQlc95S34YUsN6BQKG8pVNeNDGKm2FwDsGkdlHDarQCRjZ+rb7zszFUwC60HtRoEcdG6lh5L2huyTSpPC0wFAMo0DnHK153Wk6fpMigsialVqVcAGChDiwHO4YIGHNnFNeZm0hzWMiyvLT80ErrO+tZ7a0jHL9fKFqL9d1YovUqlEiZVDaDMK1oiKmBiaMVC1A7JKAWMIo+J1ieLeMCRi9FC/StdXo3xk7hhGXO5m4sh9OxVd/2RPF7xTFNQOij/ujGc3jwYxrfwWMIASgzE1iQF62PFO+sbyreN1fnP+pYo3AQh9BvinrHQqYxXt9us15WtBqM8cn38rAiuJzOvB5RyO9XzMfnWv/Gb2bU3kn/N5IY1WoCSdMdanS/GQQdx/JMyqGfYjsN8SYwF2iTyBh8JBPZSTXwj4WdLsbnzHWqvwqVYAjNaK1P2xdYZX3efc4bjt3aq1ALXuRJ//K4XuxQUrZat3d/zxvq1j2a/Cb8jvqCJp2zGDhf0dC2GumJ2WhtKAdngTq4xTqnvLBIoWXivxQLDtYSS8yP/wDRe9d9dKb05wAAAABJRU5ErkJggg==>

[image14]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADYAAAAYCAYAAACx4w6bAAABtElEQVR4Xu2WzSsFURjGXwtFvvJRUuQjJQtRrGQpUUJY+PgDRNZsr2TB0kZsZOFPsCLdssOGYmMhUlY2YiXxPPPeuZ1773yYjRk5v/p15753zm2eOe85MyIWi+U/0wJn8osGHXAL7sE5WJr7c7LohEvwFH7Cg9yfs0zBW9gDy+E6PIZV5klJgsEmYD98Eu9gTfAOzhu1angBl41aImmAD+IdjIHeYa9RK4KHMC06gw6VcET0ThB+jor+eVwEBduWwmCE5z7DNn5hul24Jjr1HLQDV0V7uFXH/DpBwVjzC5atD8MF2A1f4b5ov57AF9Ge92MWPkbwErY7I8PxC8aJSMsPgi2KhpqEH3BAtF/ZigzN4zjwC1YmetNDg7mwBW9gnVmMEb9gxDOAV70CnonuKlFmqET0An5qPSx2RoYTFGxD/INxn2h0C1xHXE9ca1FohtMRHIc1zshwgoKNiT68B40ab/JRRh47mOsrKbjBvLqoFp7DlFHjpsTZynkFS8Er0QFxw1ngBXJGvjK+wWvYZZzXB+/himg38K1jU/JanVOXfVr/IbhDDom+grkvFxaLxWIp4BtAXWeEn76KoAAAAABJRU5ErkJggg==>

[image15]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA8AAAAXCAYAAADUUxW8AAAAjUlEQVR4XmNgGAVDEHADsTi6IBJgBGIpIGZGlwABUSBeBcQm6BIMEI0JQDwZiFlRpRBABoh3ALEZkhhRGmEA2QCSNMIAzIApDCRqBAGQjUVA/BqIrdDk8AKQxhwGiI1yQLyeATUMcAJkjTCnSjAQYQBIYxYQT2DA9CNBA7SAuIkBUyMMCAFxF5QeBXQHAMRmESwieeNfAAAAAElFTkSuQmCC>

[image16]: <data:image/png;base64,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>

[image17]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGIAAAAXCAYAAADwSpp8AAADe0lEQVR4Xu2YS6hNURzGP3lEFCEichNJ8n5NKEooA/JIYnBjwIRCEkVRQh5F8i5X8iiKyR14ZEBJMlEMDUgJoYSZx/fd/133rLP22ufsfbjnRPtXX6ez19r7rPX/1n+t/z5AQUFBQUFBdYZQu6iz1B5qVHlzXelFNVP9gutiI3WAGg0bs69BVNdS17rQhZpJHadOUiuo3mU9kvSlLlDjw4YZ1F1qNjWRaqV+UVthP1QP+lMrqRbqM/UKFtyQi7CxxfSCGljq2ul0o45R+6hhsMXxgHpOjfD6+SieO6hv1FS/QavvFrUWpdU0gHqCSOdOREYspqZR1xA3og9skVyGZa5TC/WWWtbRsz6MpT5Sd2BjE6thi0IZEkPZ8wmR2GqymvQXWDY4dsIeuMW7FkMBlNKQuUORL7O06mNG6LvSv2dwXZl0CPl+428whnpHPYMtXrEUFrfzrpOHtqRzsMWTMKI7LL1uo3zi22EP1GcllI43EU9FPXs3tQ35gpRmhAyfElwbR12BTbIR6Bxz2aA5nqB+wgzxUZsWtbJWMU0YEUN73w3qBzWnvCnKJJiRvhm1miDSjAhRAK7D0r3RaI4LYNvOYdj8fXQOH2m/ntkITUwdlULhA9PwzfgTE0RWI9bAskELpxqqAp9Sr3NoVdud1ZkH6/+eOoNkwaBsVSyb2r9nMkI33aMuoXoZFiIz7sP2wVpNEFmM0DgfU+vDhgai83A/rOqTOUIx2Awrax1VjdBKPk0dhVVTedH9qhZeUiODtjxkMWIhbDKzwoYGM536Dith9V4zGaUtyVHRCGeCalxXxqo8m9/RozK6X5WLMkEHaCviB3gWshghw1MnE0FzUmD0zKxyh3AaE2Dj0KdD92nsbmwbkNzyvsIKIW1lD2HjakPpo5c3pZC/nSjtw9M/hm+Cu18m1mpGNSO0ZWr71LtD1szTPYuo5TmkOVRC41RA9elQ8GWC3i/S7o9mhALXDEunNyh3ThVAtdSXCQeRNFHUaoYmprHobTWGDsMXqGxWPVBAP8C2SYcKCJlzCulFhN7RFG9tYx24VHJ/E/jKsuLmUpuQNMGhamUv1SNsCFB6Kk1d2koqn2WI0ttnMOwcarQRKhiuwrJzHWxX0fj1z0DsvUYHuLYjf36P4G1N/yL6w0z/BKQtgHqh32+ilrRreFlrQUFBQUFBwf/Bb04SyU8OBFe2AAAAAElFTkSuQmCC>

[image18]: <data:image/png;base64,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>

[image19]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGIAAAAXCAYAAADwSpp8AAAC8klEQVR4Xu2YS6gOYRzGH7lE7pcUEck1cgnZiZJrLomQs1AWrEgnlIWiJGFBIpcFSeysLEg6scNG2WFBUggRFsrlefznn5kx830zo/lOR++vfp3zzbzzNe/zn/cyHxAIBAKBQHMG0J30HD1AJ9JuiRatow/dQgeljqeZRRenD7YYZTSPnqSn6XraN9HCKJTvDNpBF9DBdBv9RtuR0bgmhtAN9CL9QJ/TEfEGEer0HvqA/qR7k6dbSg96gh6io+gEepc+pmNi7Qrnq2p+p6uiz2qsjr6jU7xRzagQq+kceg2NC7GSLqdf0LmFUDbK6BbtFx3bDHtAlKlTON/jsIu3Rp/703v0E6yajVCAMo/udCTKjaxLyC+EMxudX4hJ9DV9RIdGx9bCsrzgjVAi3550GCw0MQ02PXTgT6Xz0HC8juRQdPS9++lu/J+FEFrHPCP18RT9ASuIUylfLSpX6As6M3UuD7W7iWQxqhZBdKVCOOrjEvqeHoP1P4um+Wql19ysBs9gX+oVLEK8GP9SBFFHIcbTh7D+FXXT7yubswjW/g09C3v601TKdzJ9RS8jeyuWh4pxh55H9SKIOgrRChTsYdi0o+LkUThfBajhowVme+pcIzQStENQxcelzpWhqxZCzKVfYVvY4alzTma+Cm9XZHxeUwfVUKEUQdcehY2EqfQGshfwItRRCD2tCkbfWdTchTRiOuzB019H1+nedW+6x8L5eof8QkcN0vvhPOJF8OlI++OqxaijEJoCVtB1JWz2DuUZxR9Wvy9/Ryic72j6BPZGOzA6pj3xfdhcp58RGqEiHIFV3IvgVC2GbvIl7G01D+/gvvSJFqKH4C1dGjvWBgv4DOzNu1S+y+hTWKAbYeF9jI43YyHdgb+L4Gi3cpD2Sp9IoWlDLzmfYR2RehtVQeLrlP7XQudtpLaMujZvTq4LBXuV3oa9rLXD7l+7Iw9dlMq3N+y3EA3J+cjfBweS6AEcS9dEagRkEfINBAKBQKAr8wuzR8pc5mviTwAAAABJRU5ErkJggg==>

[image20]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAYCAYAAADzoH0MAAABDElEQVR4XmNgGAXowAqIHwHxfyT8BYifQdl/gXgrEKvBNOACk4D4GxCboomrAvFdIL4OxDJocnDAA8QHgPgqEIugSoHBQgaIa3zRJWBACYifA/F8IGZEk4MZ/hOILVGlEMCPAWJDOroEEIQzQMJhChCzoMnBQSsDxAZPIJaEYnkgrgfil0AcCcTMcNVoAObE1wwQL8yC4rkMkDBpAWI+mGJsAJ//FRggMXAOiMVQpRAAn/9BABYDIO9hBbjiHwQ4gXgHEP8DYhc0OTAgFP+2DJDA3cUAUYsBNIH4LRAvZUD1PyjEQU5+D8RXGCAxggJAJj9kQKR9UDw/YYDkCRD9B4gfAHEuA8Qbo2D4AQDFDjxnJ33hQQAAAABJRU5ErkJggg==>

[image21]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA8AAAAXCAYAAADUUxW8AAAA3ElEQVR4XmNgGNZAEoirgFgAXYIQYATiZiB+AsQyaHIEgTEQf4ViEJtowAnEi4H4ERD/BmIbVGn8IAiIu4G4Aoj/A7EvqjRuIAbEa4FYFojLGSCao1FU4AGlQBwDZYNsBGkGGUIQaDNA/MoD5cM0t8JV4ACsQDwFiG2RxEABBQqwhUhiGIAfiJcDsQqaOCiRPATiAwwI12CAIiBORxdkQGi+C8TiaHLgVARy2gUglkOTAwGQi44zQAwAGQQHIL+9Z4AECAg/B2ItJPk+IP6FJA9izwViNiQ1o2BwAwBqgCY8YTiPnwAAAABJRU5ErkJggg==>

[image22]: <data:image/png;base64,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>

[image23]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAAAXCAYAAACoNQllAAADRElEQVR4Xu2XS6hNURzGP6HI+50iuskj5JVE6A7IozxCYXBHEgMR8hwdYUApSUjqkpQ8MlIG4sgMYUDKoxDKQKIoJL7v/Nc6e+3VPY+Lzj26+6tfnbPWOvvu9a31f1wg0z9VV7KWHCSLSYf0dPtWL3KBzCBjyX3SkFrRzrWBHIXdmgFkDexGtRv1JJvISbKHjEQSQt3IdTe+kewk/d1crTWYXCYL3eeY7slSjIKlA+1pq5uPpT1OI0fIMTLbjaU0geRJI+lD1pHvsIdq8SDyAmaSQm0iuUkGovaaQr6QXyXQu0vLYUZqb7PIA9ielrl5qTPJkTtkPBlDbsEuQsokufcTlnglmXSXfID9yBu0w83rJF4iWV9LLSL3YLciJI/kAPW+t8kcJBsdQd6RZ2SoG5NZMm2++y7NhO17ejCGQzD3lVekHrA/8Bl2Aj7EQoNeITmtWmozbOOhZMpFWPGQ/C3TocosSUadg+1zAelCrrp1Wu/l96ZLU5SumnJKR/d9HPkIOxUf0zmy233WQx4jcrlGmkz6Bt+18QOwkPKSYQqvZqRz0hmYQbqFGs+jtEGaC39blJK1nH4NyzVew8gV2OkpRrehhWTWBlKYn4Adcjn5tPEelrwrGRTevoIURudhxmhyHpIb5aWX0I96R+OltBr2vGpRblGuqFb9yA1Ud5NXwvLsdiQHmyNfyVT3XVIO+gEzqaWqV9BoWEI7CzOuXrWKPELllkO3/wnMkPCmafwp2QUzTXOnYGFY1qAwoa2P5upFalKvkUukUzQXSvlIyVg9XhwR0nBYAXoLawWaEOUguabKIEJ3VbFkkBLbn0qVwjdw1aDwrZRLvNR+qByXez9vjsLLh5UawTC3xvLPLVYxXxLjZOUzfqrctVK6witawRKkK1Q5qVTr/fbHE04y+jDSjaG0D0nOUQvzkCxNpgtJX3mpmNfUNKl5Og1zXFLyU3epUj/JjdWb1IPJIN+bhfId8jeki8AbWIPb4Nap3OsZWitp/wq344jCVqfxHNZPKPHpWn5y4/WqLShtULl/R1TqVfKlIbDKuRfWJOtzM0oUJuWLRthVV5xWmwvaStrEXFjf9jfy+1aY+X9BMmXKlCnT/6DfTmG4/p/YVxEAAAAASUVORK5CYII=>

[image24]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGQAAAAXCAYAAAD9VOo7AAAEp0lEQVR4Xu2ZWcitUxzGHyFknlOGc07mWUgu6JAMF6ZzzIRIXCghRC5IkumCZMpwJKGMoSQXJ4SQoXBhyJAhhAgXyvD8vv/6n732e/a7v72/9O2t3qeevr3Wu9Z61/rP6/2kDv9brGIeaN5oXmqu3f+4w3xjqXmnubp5gHlf+d1hAljXfNk8t7S3MN81914xosN/jvXMC8y7zavN7RVhCqCAL8wjW9qTwlrmmeYGjf5EfaYrSnuumG0t9nKK4vkN5s7qyS9Bez/zVvN283i1hP49zOXmYnNDhSf8aV6sWGRN80nzshiuRea3Zdx8YyPzRHOZ+bPCMDCQJnYyPzQvUjxnr7QxtHFxsPmxebpirUvMJxRKAOuXdj5Hni+pJz+wmnmLea25pbldGfO+uU0ZswJo7C/zqNJGKW+aPyoOBvYxnzJ3M69SjJ+Eh6CQoxX7eUSDFbKOwsCeV09oCOQx8+Hye1TsYn6tMAKwufmp+t97vsJrauxqvmMuLG3kiDxfUOwPnGr+o5B/H24uD84u7cwZvyq0ncBT2NDWmo4c8oAGK+QQ82/F8xp4eG1kswHF3WN+YG5S+rB4BHlWeQ54z/XlWYI9oZCU3w7md+Z75salb4lC7ryjD1RLvHDV0ka7hIPl6mnzSvPeMuYg81H1rG9SaFMInstBBymE/lE9G+v+SuFZa5ibKYRZCx4QhtLSMyfgUc+pJz9Arss2a9ymMBwU0wqS1UPml+aeVf95igWOVeSTleLeBDBXhZBXRkF62rPmXYp5hLxXFHkggSw+Uqz9iUJBRJg2GaGMw8yfzJvUcn1As8RkFEGMZEJ6DGARrIOQNXCBAThZsd6ofMvcdmbmaGhTSFo2hpXWnDkEoWVxMhtSsb+rF545OwZZ5wJAGOQMjE9jaFZiACUz7nuFkjMUDsWO5jfmg2opy6YEbQpBCXxNoDJaUPooN7HIuSiE0EP+TDAfz0G4AG/JexqVFgpk3qvq5YsmMPbrFKkh12kFB8K6WJRQNa1oUwjgwFQ/WOPnirr/Go2XQ47Q8NDHX/LoM+V3AgU9Xcawhzbsa/6hKH3JTzPABS8srENRvrS5mXGAVSGsUTlOOATDFDIIJN1xqiwqJCrNpgxqhTCG/NFcE0VRdufc3RXv52+CfbP/OiTO/KCjr1OxEC9dqUYeAyS148Yg9wvuGaOiTSHkC0r5xxWXNpB3k+Y9hMqn7abP3NcVuaeeU4csZMaFc1H1PMHdJOWX8qyVm7LvM5KtFLF2mXqbJ+69oYhve5W+aQSHI3nXFQ9I4fNsYelbqvi6UJ8n7wb1uCYI2XgAcgLNpA75fbn6y+FNFXvYv7RR4g/m4TnAOE2hpDvUuKwSKynXuNycpEhiv5T+aQOxlgT6m3oVDV8NEGqd7/hs8ZriEoeVfqb4BFIDRXJRw9rb8gpFzf3m2+YZCg9jDt6foDKkD8XwPjyDyyQJPpWEsTP3RcUFnP1xBirbdIQ+EO8XK0IH//cYJ5ZPIxDEAvMYzX6eczTc+FiLb2DDZEMRQQhizKEaXJ3We4LpdR0qECq4nLWFrA7zDGJ68ztUhwkB7zhBLTG8Q4cO04Z/AVxCDeubZLD1AAAAAElFTkSuQmCC>

[image25]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA4AAAAYCAYAAADKx8xXAAAAuUlEQVR4XmNgGDnACYjvAvEjIrELSBMjEE8B4pVArADlg8AcIP4HxB5QPjMQ2wPxAyA2BQmIA/EqIBaDKgABQSA+zQBRJI0kzgPEi4FYBsQBWVuIJAkC+kD8CYjXADELkjjIwElAzAvihAKxGpIkCEQD8X8gLkcTFwbiNAaEdzAAyH+/gdgGXQIfwOU/gsAYiL8yYPqPIMDlP7wA5On5DIPWf6A4PAfE7xggfoPhL0B8nQFi2CgY3AAAzMQr+zx1NKQAAAAASUVORK5CYII=>

[image26]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABkAAAAXCAYAAAD+4+QTAAAAqUlEQVR4XmNgGAXDHRgCsRu6IDWAORCXAfFpIP4PxOWo0viBOBDPAmJudAk0ALLEF4i9gPgrA4mWSALxQiDmQZfAAYwZRi1hoKIlAgwQQ5GxPhCvBmIVLHLYLMZrCSj1VDNAUhIyXgrE94F4Pha5ZLBOVIDXElyA6sGFDQxqS6rQJfABYi3JAOJnDJAiBYbfAfFhIBZDUocVEGsJRQBkeCgQs6BLjIKhDwBefCwhOsXpIgAAAABJRU5ErkJggg==>

[image27]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA8AAAAZCAYAAADuWXTMAAABAUlEQVR4XmNgGLmgE4h/AfF/JHwRiNWg8lZA/AVJ7i8QZ0HlwEAciO8C8XUoGx0wAvEiIM4FYmY0OQYbIP4NxEsZIArRAS8QrwJiJXQJEEhngDgJRGMDmkC8Goh50CVANoFs/ArExmhyMOAHxM3ogiAgAsRXoRjExgYmMEAMwACmQPwNiOczYPcvyKnLGcj0L0gTSDOGf0GAbM2wwAI5G+R8bABnYIFMO8AASSC4EkcfEDugiYMBNxDvAeLDDJCEgA7MgXg2ELOiS8BABhC/BWIdNHGQN7YAsTyaOAoAmdoDxO+BuAaIk4F4JwMkRUkgqcMLZIE4AIh9GbD7fxQMGAAA4Dkuapwn0scAAAAASUVORK5CYII=>

[image28]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAoAAAAWCAYAAAD5Jg1dAAAAvElEQVR4Xt3RMQtBURjG8degKCWTmYFsSlaTlRQTvoeyynK/gLL5ECYDozJjt5hsLBb+p/Oe2+F0PwBP/brd5z73DveI/FayGGKJCNXPxzZ5bDBDDnWc0PdHJhMcUPC6Ec4ousI8NKOVKzRN3NF1RQ03CYcNPDD/LpKGcd/Byy80wTAokvoyrn6hccOpK8x/22GNjCtJG0+9xhnjgpLep8T+/L3Yw4iTxgJb9HR0FHtCQcxXKhigJfbl/8wb4ZAlMSoxI0oAAAAASUVORK5CYII=>

[image29]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA8AAAAYCAYAAAAlBadpAAAA0ElEQVR4XmNgGAWeQPyfCPwViI2hejDAQiD+DcQ2aOLMQJwGxM+AWBNNDgwEgfg0EN8FYnE0ORAQAeKtQCyJLgEC+kD8CYjXADELVIwRiLmhbJDm6UDMA+WjgGgGiL+KkMRATuxhgBgiAMShUDYGmMOA6l+QP9uBOB2uAgeA+RdkMyhQvkDZ34DYFEkdVgAKflA0IPsX5OQ9DBC/4gUw/5YjiSH7FycASc5nwIxfVgZESOMEhOIXL8DmX4LAhQESsshp9x0Q7wBiISR1o2DoAgAXri9QT5a+dQAAAABJRU5ErkJggg==>

[image30]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABkAAAAXCAYAAAD+4+QTAAAAeklEQVR4XmNgGAXDHRgCsRu6IDWAORCXAfFpIP4PxOWo0tQBIEt8gdgLiL8y0MgSGDBmGLWEBEDQElYgFgdiSSKwGBAzQ7ShAIKWGADxLCLxRCBWAOtCBQQtoQYYfpZUoUtQA2QA8TMGSJECw++A+DADJJGMglFAQwAAAhwiR5xvzxAAAAAASUVORK5CYII=>

[image31]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAAiCAYAAADiWIUQAAADY0lEQVR4Xu3cy6uuUxwH8KUQkUt05DY4ZSIZyEB0TESZkDBQxi4DA5QUZWZiYqAzIpIMRFIupZRd/gYlpQ65lAElJuSyvq13OWuvnnfvd297n8LnU7/2s9Zz2e+zRt/Wep6nFAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAPjfOGPuOARnzR0AAHvxba2n5s59OLPWr7X+XNXPtb6p9ch40CnwQq1n5s41jtd6eO4cZGw28WGt30u7769LG4fPat2z2n9zrXdW2wAAe3Jbrd9qvTXvmNxe65K5c8GdtR4a2tfX+qXWuUPfYXug1l1z54Kzy85hso/N6fOONZ6sdevQvqO0ANfdWOvo0AYA2Mgbtb6stTX1jxJsXqp137xjwbOlhbQuAS6hZdPQcyrld149d64kYPax2SRs5v4Sei8f+hLgxsB2Ya37hzYAwK6urHVRaWEtwWSd50qbHXp63rHgu1qX1bq0tGXAt2tdse2I/XustACUUJjw+ENpS44PljaL92itC0pbjuz38+PqnFeGc7pXh+3Z6+Xk2ORednOstNm4HJu6u7Sl2fPGg6pPpjYAwFo31HpxtZ2ZoQSedbIcGplFSnDbyXid00oLS1miPCi5fl9yTShLsIqEr63Vdma2emDrS7LdGEy3hu1Rxqa/iJCxGWcM18lxJ4Z2zk9wHJdII4EWAGAj75aTLwek/ti++295bu38oZ3ZtgSxJVk6/HTqS1j6aOr7J3K9PBsWCV99luwgA9s8NnPoWnKi1vtTX87NUvLop6kNALAoS5RHhvb8rNUoS4mjzLatWxqdXzjILFPenEz/7Npa9+5QeW5uyUEGtqUXLZbG5vGhnX1LnwFJ4B2DXZZTsxx73dAXX0xtAIBt8i2w10oLZ8+v+hKc+kzSV7VuWvVHlkCvGtrdGMoi180nPHKNPCOW6+Tve+NBByC/uf/WJ4btPMjftxP2xpmx/qmNN4dzPi/tvhMkLy5N7qGft25sIsFsnDFMMOufMvm+tOMyFn25eZTxTAAEADgwmUlaWv48Z+74lzpa2ssCe5GAl5cJ9iP/b7dnAAEAmHxc65q5cwcvl70d32Um7oO5EwCAzdwydxyCfHpkabYSAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACA/46/AAUipMZ7M8LAAAAAAElFTkSuQmCC>

[image32]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAAiCAYAAADiWIUQAAADo0lEQVR4Xu3cS8htYxgH8PfEKaLkkkuUS5i4hYGITCgGDKSQMnNJDNwyY2RMSklKBsq9JAYy+DJkQilyKSRCjBi4e/+ttZx3vWfv/e1zvnPq65zfr572Ws/77rXWXqOn511rlwIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHlSP7xH5wVJ8AAFjHd31iL91S45ca/46fP9b4o8aJ7aRt6pQa5/TJxu81buiTC1xWht+f+HX8/LvG2c2cp5ttAIBNXV3jzxqH9gPVszVO75PVTX2icXKNt5r9dK1+q3Fxk9tuzq3xaZ9s3FWGwuuxfmCJjRrHdbl8f3Jz0WkDANaUYuqlGl+P271ratzX5TLvhC7XurzGw81+Crhvy+LCb7vI9bZFZu+1MhSdz/cDS3xfY0ezn3uWDt3kjDLcJwCATb1Q49gydIROmg/97+5u/6luv5Uu3as1LirD8c6q8XmZFy9bkeIxnarnytClyrLrizXuKENBNfmpDOe/rexaxszvmJYnrxy3vxjHUrBeN2730hncWYY5G/OhpVKc5fyJR2v8UOOQ2Yzh3u+r+wIAHKAuKUMhEimyVi1ZptMWKTAubQc66RplebWVQurwLrcVbWGWImoqetL9mrqEx4yf0c7PMuS7NZ5ocvFNWfz7s/T7yLj9QRnOt5l0z1JQTk6r8WWN45tc5HhHdzkAgJk3y66H4xNXzYdnpq5aljVXFV9ZWvyny23UuKDLbUVfsE3agi0vEKRISnHWzo+8WPBOl1tWsH1c5vfo5/nwQtfXuLPL5dqu7XK5vlVLywDAQS4FTdvxSaF1f7PfS1cty3tt52iRr8ruz4Kl0Fn0QsOpNW5cEW2XrLVZwXZYjU+afOY/WOOIMnTYXq7xTI0zmzl5S7YvqC4s8+I0x89vmeRc/ZJmchtl/sLBFWVYIu2fEUwxmWsCAJhJMTN1ix4fc+c1uXSalvmorO4I5S88coz8jUeOk2fF0qHqn93ailxzzvFKjYfG7c9q3Dpu52800inMs2kpyl4vw3U/UOONcU46WynOsp1rTNGUQvTJMsgzfe+N41OnLMec7tGHZSi+3i7DNUxSmE1z8pJF4q8a95bdu5JZCk0nDgBgnzq/T2xj6Xylg5hicZ2CMS8wvN8n13BPn1hTOpZZXgYAYA+k49V3wlbJc3l7Mr+V5VAAAPZCnnXb327vEwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABxQ/gNo8JzxS2dOVAAAAABJRU5ErkJggg==>

[image33]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAAiCAYAAADiWIUQAAABXElEQVR4Xu3aoUpEURAG4CsoKAaDIgg+gWCyajQYbFa7WRCTXRBMJsEgBvEBrBZfwBcwCILRoklEZ9wLe/bABmFdw/0++OHOzIG9cdhzmwYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAKBnKnJfN8dsIvIcWa4HAABdtxi5i7zUgzG7inxE1usBAEDXHUT2Iu/1YMxOIl+R7XoAANBlk5GN9jmXpf+SS2PKdzgsBwAAXbJW1XOR66Ie5cI2H1kakuniXDpr+ktjXoleFjMAgE6pF7a8Bs0lrcyoHEfOh2SrOJc+m8F3uB0cAwB0R7mw5VXoZlGn/IZttuql1cjOkOTyNdM/+mv7Vf0Ueax6AACds9v0/sl6iCw0vSXtpu29RY76R/9MXoG+Nr3fXGl7p22duWh7AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMCPbzy8L/EP/+GsAAAAAElFTkSuQmCC>

[image34]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAAiCAYAAADiWIUQAAAE0ElEQVR4Xu3cTehtUxjH8eeGIoTIJcQVSvISoaRcQuStJJGXiQFJiUQxuROZkXd5SUjyloEMmDgjBiYUE5JLIgYGYoC8PD9rPXc/Z/33Of/9P2f3P3fw/dTT2XvtfdY5a5/BfnrW2scMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIDR7NU2rMgeHlvaxiWselz6/APaRgAAsFrHedzfNi7heo9/a/xSX//0OCyftKQnPW5rG5MfbNwxxTgUP1sZz3Pp+Mtpexkftg1LOtTKd/62PZBc5vGYx8Wp7WCPs9I+AABYsc893vLYsz1QqYL0qZUkZafH2VNH++302Dvt7+fxe9pfxj4ed7SNyUUef1kZ0zxbPZ5tG2c4wuM968ak8Uw8zqj7unY76vaiLrHyncZ2n81O2C70eLpuP+hxazr2UdoGAAArpMRDCc6kbrc0NfaSlSRJrzrncVt/2k4JU6aE5/umbVFKkk5sGyt9v9etJCiT6UNrHG5lTEOcayXxCRrPTo9tqe0Tj4PS/kZoSvXFtnEk8xK27R7v1G1VJO/uDv2fwM1K4gEAwCY5yuNVK0mObuhKYFr3WLc+KxI2ecLjpLrd0k3+Ryv9KVTF+srGW+c1L8nSeDSdN7HZSUrYSMKmat3pVt5zvJXxXDB1Rvm8q5u2oZSEfpf2j7EylfmMlSTqa487rVz3Dzw+q+dpmvl9j6Nr2821XRXInzyus/Le9a7FLVYqoPk30nc6M+0DAIAV0BosVcqUYOlmHdN7QdUiJSeReL1pZb2btrd7PLrrzGltNUpTqOpfVboxTNqGSp8TlT8lWH1TsAdaN55TrYwp9vsS1tBWDNX3203bxEpVahFX2NqkSsnkIXVb1zOSJ/1OMbZ/rJvG1PdXHxpjrvbNq7AF/Ta6Zrlyqf5uSPsAAGCT6QYdi+gVuvFrPVOmG7am6VQhU3yT9lXJeqo7dYoShNxXrPdSgjSGSdtQvWtrx9R6wLrxaAwaU+zPW8/W9jXx+LWn7fmmbahZCVtUNHVNI6HOCZvGqfdKJGyn2PQU95CETVQd1DUM6i9PkQIAgE32RrOvG3/fzVkL4UNOIO5qjoVY25UfOLjRSv9966E0lXfNjLgqnZf1PUxwpJUnIoOSFH3mPEOnROOBg0x9x2L9oKpW3zUcQlVJPdmaDUnY/rDuadhjrUxFKxn/wqarc30JWyTSUQ1V4qf3BfV3adoHAACb5BXrKlDhy9R2TmoXVV3iLysigdC02Wm7zujcZF0/Wo+lBORvj2utPGk6liutS0aUGMZnPlLbTk5teV1Ya0jCpr/wUD96QjbGpKdq+8ajZEfJ3SIieQoxht+sS3j1HR6ur9rXdK6Ss3s9XvD42OMEvdldbuUhD41PofNvr8cyJbrq5yEr/eaHSTQdGgkjAADYzZ1npYqjG/v5Nv5/hW3UNisVqWUNSdg24jXrryIOtcMWf//WtsFKUqn1bEq6cvWxpQRc1TU9rJFpPAAAAAtT0jjrKdVVUBVsvb86GUIVtN3BFiuVOwAAgKW0f6uxKvtbmVocw75W1o6tkqZ1F33aFQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAwvv8AscfX+LAZuFIAAAAASUVORK5CYII=>

[image35]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADUAAAAXCAYAAACrggdNAAAB0UlEQVR4Xu2WMShFURjHP6GIEINMhMVECQtlwEpYsEjJYkERJUnKQjFJGZBNMpDEIDZZlWQhpQwGxSb+/3vOue/ew9N7DPe+3F/9Xu+ec173+9757neuSISkwTrYZk+kKr1wH17BCWsu5dmQFEqqBE7DNTgLK/3TLmFIKltU5TDWFdgC030rQD08hk2wGh7ADzgm6jnyEnRS+XAXDsIKOAPf4aGec2DWe3BAYtkWwQv4Cmv1mCHopIbhJszT1/zT50RtghsXy+4OvojaJcOUqIWjnjESdFK8vx0XO/IbPIE5HMiEy/BIVIIGBu7LXhN0Uh2iOnCrZ4zVxKo6hbmecR8ZcEdUrTbrsRq4Dh/hNVyABXouaPpEbcCiPeGlQVTm7C7cyd/SA++T8FLid914sDmcwRtYas25cBFrc0t0fYYYNolxeAurrDkX7soqXBLVFcNOFzyHZda4i0loUmKtndn/5T0vS1TzSdRiSbzcmRDPpkJ9zQbBZ4v3dOA28qAd0d8NQ7DTc50srPHuJGyXWJA/wZeFbfEctqI2gF3ciZ8f/aL6/IP4H9xn2Oj8JDwweD5DT/I11nmzyBy+bIm2bN/lZmFIMIfvd7L8IiIiIv4pn+7HabCvgEJ/AAAAAElFTkSuQmCC>

[image36]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADUAAAAXCAYAAACrggdNAAAB70lEQVR4Xu2WzyulURjHv0IRUaNIadxkY6GZElKUxaSspvxYWCjZmKUfC41YWKkZlN002SmrKTYWZiQLWcjKf0CiLCyE3cT32/Oee9/3ul33tbjvK/dbn3rPOc9b5znn+XGAd65iMkh+kzlSFVx+m5og3TDn5skhqQlYvDFVkgOy7o2byCXpdwZxVT1ZgIXXImkOLqOVNHrfLTCnvqSWI1MRmSTf0hc6yD/SQz6RHfJIZmA/pWuW/IXdYNTqJPewPSVVTrbJOCxfJOXKMcy4zZtz0gFskg9p81GomuzDLiDglMLujNzCbslJFU7G0765z2SJVMCcqvWt5VuKIO3tJzLcVClZI7swB51k5D8B5dMP8hFmN0ravbUopLBbJl3I4FQmlZA/5D/pJWVkC+ak44I0ePb5lsJOBS0BS4+cnHLJpx91k6/VCDkPwQmeV910KeymyLA3zskpncIe2YDlTtykQrWC1GG/6JQMf5FVWFWMm9RCtL+Eby6rU86h70iVdjXZvqRFeCkXVVRypQ7Zw10PgFMEQ/YaluN33jj5IFCcqtEqVv3NVu+9Ad84rFQxh0LwFeH7X8abkhNj5AFW0fyncAN7xMZZaivau/pqUrp2NV9/uXZcwR6vcZQa/xGs7bj9KhTj8B4tqKCCCopAT8mxat4VR/okAAAAAElFTkSuQmCC>

[image37]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEMAAAAXCAYAAABQ1fKSAAAC0UlEQVR4Xu2XS6hNURzGPyHk3VWSV+SRkVtI8kiSSCSPEmJmcBkYXWFAyYBSIpSJJCOmtxTlykRMPYokUqJMxESJ7/Nf/3PWXfvsffbRude92V/9Omf911pnr/+31+sAlSpVqvR3GkP2kGvkHFnYt7omxVWvdmqvfkNFw8hycpFcIZvJ8D4tqInkHjlNxpFO8oLsiBuFsuKqVzu1Vz/1H+ySEd3kIZlDOsgt2EsdGbXDUfKUTI5ie8lLMjWUZ5LXIe5Se/U7HMUGq5aQT2RVFJtL3pGNHvCEbnggaBn5RraGskz4DvtRl9yWu72wmTIQGk0OovXnnYElPi2KjSePyHVYLlhEviBrhpJW8voRSessNUNSv48wlwdCGrTGlI6jSDKwB1kzZGgvolXhSeeZ4XF95pnRKN6f0rPOkxFpRY486TwzavEt5BeKzfBOjZL+F2Zodhwh+8P3ZlKiSripGZvQ3Iyx5H4op0mnZsyATWMdXWtQbrCTYINphVmwtX6JTECxdAi8QQkz0uXgSuNp0q44LiNOwu4e88kr2F2kSDL6BOyIa5W7sGfriB+FfGWSzotr49MGmGfG8VDW284z4wPMCC25r2RxqFMfzSgl3G4tJbdhz20m7S13kG+GThSdLLVAD2zXda0nP8KnpCP2Z1SWfJf2vkp6JeqXGJ1A6e+2Qx3kJpmdVhRIdymdmjo9XVPIc9g4a9pH3sNuZpLWuabeY9RvlxrAE3IqlKV5sFmxO4q5dEl7hvo9pZ3qQuNnFqnRWFeTz2RFFPvzJq+SB2QbzAg5pmt3LE3Nt7Br7U7Y+XwWyXU2lC+QXSi3gbYizbLLZHpaUULbYePXpe0A7K+FjM2MUYEFsCR1CqQJurQUNsBM09tPpX7aZ9aFsjbSsveBMtI4tb4zCZSUZrj2NqHv/SYN8BDsQdqo9A/3WIj/d1oL22R1b3H8NKpUqdLQ12+ywZ79KTiyqwAAAABJRU5ErkJggg==>

[image38]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEMAAAAXCAYAAABQ1fKSAAACUUlEQVR4Xu2XPWgUQRiGX1HRoEZNQBF/ooKKVQLGSpAgNhZJBI2GKJJKRBu7oBYWkkZQopIINio2grUgJGAgTcDWH7CLqKmMIGohSHzffDOwmd3R3cvB3eE+8HDZb/f2Zt6dnZkAJSUlJZXRRAfoA3qT7lt8ehHL6Sm6M6g3CurrIN0Q1BdYT8fpDbqWdtC39ETiGt2gm96hn+gPeiBxvt5poafpI/qVztAtyQs8Q/QV3ZionaHv6GZ3rDCO0S56HY0ZRi/tpE8RCUMBKIjHQf0g/U57grpQeLUKYzU9DxvBlaK+Zoaxn35BOgx1VB0eDuqilmEsg7VpKb8dDcN3OhZGWBe1DEPod2/RFeGJnETD0KQ4j3Sn6zkMjY7L9Jz7uyjRMDQpVjOMbbBhPEYPI19jtcSpYUXcQR/Se7QZxYiGEet0rC5iYSgIrTRaefbQ97C9y99YQ6/B9jdFfQFrh7YEq5CfaBi76SzSnfZhXA3qIhaGXrlvtN0da4RMwDpcbbREPoM9gKJEw9ASNUmfw5Ytz1H6y32GxMJQpw/Rle74LtL3rQat9AltC0/kJBqGOEs/0F3uWO+5ht40bHcaojB+wvYiMbbT18jepyyVi7Q/LBZAYXxEZFTpSd6nL+lxWBBvYNtyj566dm5zsAnX+5neTlwndL8R2od8E2gRNMpG6dbwxD/YRKdgG0nf9t+wUC4krltAjd5LT8JWAT/Ui6LvaZ454o41kVa6H8hC7VznPusaNfASbCLV+6j/fK+4+n9HF2zoJV+jrNWopKSkMfkD1y6Br6XcY0YAAAAASUVORK5CYII=>

[image39]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEMAAAAXCAYAAABQ1fKSAAACmklEQVR4Xu2XTchNQRjHHyHkm5V85iNZiPKxoCRhRwolyo4FFrLwhgUlC0r5iIWNkJWyE0W6sWMnLFAipSwshJ34/96Zc985c+ec9x73vtw4//p173nm3DMz//PMzHPNatWqVaszzRE74mCgheKMuCJ2ijH55p4WY2XMjP2CWC+G5+6QFol94qH4Ia7lm5vaKl6KpWKcOCnui4nhTT0qxnhb7BHzxHFzc73r25rCjC1ilfhgaTNmitdiVxCbLJ6KA0GsV8UYr4sJ/nqYuZf5U/RlN4WaJt5Z2gxM+CaWBTEeeFM0zGXKn9Bosdeq98ecmPihILZCfBcPxNgg3q8yM1hjsRmIez+KuVF8qMQLOGWt4xhMZP4LsSGI8Qzm1LCEuWVmECsyIxUfStHXWTEibqgosp1s4VktKjID1xqWnvTfMIPsOCh2+++/IzbNR+KVmB219avIDNYT6yo16diMGebS+LJYY+0NdpK5vqswS1wVF21gU2xXjOmweGPu8EiqyAwUTzoVxwiOLM7zBeZc51wvE0YfM3f2V+Weub45FUZZ+6JEeGyupipUmRm87SIzOI4xYpP4Ipb4Nn6T3Km7oOXilrl+qwgjqC2m+Gu2APYOTqmcyszYbK5IoWrLxAPuePjOpFeLkb6dEyhr66amihtWsNZLtNJcKRAWWSyT85ZYzpkZ/CBuZABPxIkgNt9cVqTKd4q05+ZM7LaollN9lolJs0d8Eu8DPpvL4KZ420yKN89RA1/FM7E4uI/UfGtu89lmrvo8bQOZkInrc2K7tZraqciyS2J63DCIyPZsbjFhVV1JLIWN5ooY3n4sjDgq1vlrNtJO64FQmDvef/a0GOB+cxspS45/uEd8/L/TWssvNSBLatWq9W/oF1p7jdSsGqbmAAAAAElFTkSuQmCC>

[image40]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADQAAAAXCAYAAABEQGxzAAABkElEQVR4Xu2WvSuFURzHf0LIS8okr4tkkAELpZssBrKQDEYDi00YDDKSlxgskpQyK4P8ETLZFEaDsInPzzm3nue4z73uvc/NffR86tO9z/M7dc73nNN5jkhMTEzYtOO0+9KhFKfEtC1KunAer/EDj/3lb6pwDHfwEd+w19eiiNBAEziADxIcaBQTuCZFHihJI95L6kBeluTvAlXiHNa4hVREIVAJbsgv+45CIEX73cQyt+ASlUC6Sos4a/8HEkagZjFb4gCHJEOHlnoxfWdjKx7hHtZJAPkG0jB6AuqJ2IF3OONr8ZNqXMXDHLwUM451rJAU5BtIv1Mv2GOfdaWuxAw6bPrwXMwkBpJvIB34IJbb5128EHPUhkkDnmCbW3BJBjqV9HtfA71jv1vw0IK3OO4WQkBvNWmvZyNibgh67fm0vuINdts2Ovtn+Oxpoz7hlm2TRFdoGycl/cTkgq72Pja5hUKhYVZw2D7r4ZDxe5EFOkG19rfgaCcLYg4H3b6duGzfR5KE+LetqqsVE/Nf+QImxFjLFG6VAwAAAABJRU5ErkJggg==>

[image41]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA4AAAAVCAYAAAB2Wd+JAAABL0lEQVR4XtXTvytFcRjH8Ucog25kkAxiUMpmUPJjYSTxR7jJiskgyW6yGEwGykTGU0Z2UgqLyWig/Hh/znPucb6Hc47Vp17dus/36TzP99xr9i/ThC505Atl2cAbPrGdq1VmGu+Yyxeqso4nDOQLZWnDKSK0h6Xy9OIeOxjEPHqyB4qi/T7wgC3UcYuF7KHfov10MYuZ7w6sYvQWHNvPQ2rUBBq5hlXzaSbN33m6366fj9OJS9yZ3/IRRtGKPczo0AheLNxnCM84xBTO7HuaWfMJ48bH5LORFbxiAksWrqFGTWP9uMF4UujDNdbMd9HFRRY2ave4uIwr7Js36YnNycHCxkb06+k2v4BstHtkYeNFWi3JMM7Nb1rRztk3UBitone4iTGcmN/Dn6M/eLrKFyEWNi7e8NJCAAAAAElFTkSuQmCC>

[image42]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA4AAAAVCAYAAAB2Wd+JAAABDklEQVR4XtXSv0tCYRTG8RMpBDYoDhINYkPo3Kpbi0tE/QdNIa7q5BTi7u7Q5OIm0tpeg5OFIJSLU2ODSj++h3tfeO97814ce+CD+J5z8D3eK/Ivs4cs0m4hKm2s8YOOU4vNOb5w4Rbi0sISJ24hKgcY4xGHwVJ0jvGGLk5xiSO7YVt0v2+84w63mOHKbvorup/+MdfW2b3EXD2BoYSbdFBvYK6cwo1Yz9ns1zMHJIMnzFHEACNMxdr9DJ8S3KeED/EG9EYa7ZuIM7jwP03qWKFinYUGC3hF2f+exwua4r2/JqFBLdbwjL54Q/qL+6bBT2jQRN+eHJJuwc/WwbjsPKg3aeABG/GeeTXQsUt+AeFdMFNuT5bEAAAAAElFTkSuQmCC>

[image43]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGIAAAAYCAYAAAABHCipAAAC2klEQVR4Xu2YS6hNURjH//LIq5BXYoAMKI8k6YaJTCQGXhEzAxMjhdzRlgzMhFIYXAkpMVLMjpSBlKGRAYkMJEIhj/9/f3s5a6+zzz773Lp7n8H61a9ua+1973p86/vWvkAkEolEIt2ZQ5/Qv54/6VbvmUn0dvDMCa+/TqbSm7AxurH8pm+9thd0Cx2XvVMX4Vp+oitzT9i6+euoca/yH9iRdVz3Gz3G01t0mE4M+ppgDf1C79IJXvsUeh62OTu99jrR3/0MW88k35WyiD6ly8MOsY5+Q+fEHBvoDdhEB4FdsIkeCzvQDqoHdHLQVwcJPQKL9pd0fq7XgugqitcZS+l72qLT813p4o/QtUF7k1ygv+imsIMchG1Et6AaS6bRa7CovwQbx/7cEza+ogBK0a69oq/pgqDvED2N+nNuNxQoLdh4w2jTwrt6Fi5AHSigFe06iUP0B32IfCY5h+IASnGT+0hXeO2L6X06z2trGo1P4yyK+N2wk3IZzdSybfR49rMWX5ugzdCmiFn0DjoD6D+akCb2na7P2nQCtHv9Fr1l9Dl904cH0jerofEo4vU3rni2YKdkH+xy0QQJ8jdOnUqNVWlK66kgUpCEAZRDuU0vqdiJzbBf0ERklaH68IfuhaVR52xUT58zkX+3zLBmdsPVh4VemyJfBVuFewl61AfHSbRvIjNguVbRPUi4FKqJqSCOBuVvpQ//NJV52F7riV8ffBLYuh5Fj/rgcFfCs7DN0BVsNCgtqKaEkVVm1ahz9aGpq2kZSpmnwkZYML+jH+gj2IdfKcptOvI6SvpmqLo4ITqi2+mePvQvCGW4YNHpHTQS5OuDQ+nSXWWLLhgduI86qQ+4QcNNSMFSNOEmmQuL9tVhR8YQ7PZUKcsoReg74gyqF706UAoaoV9hUSX1L4zHqHDMxxil4Gdoj0teROetTVfZe6hQH4RuRxthqSUSiUQikUgkEhkw/gGCoKSu8A2KDgAAAABJRU5ErkJggg==>

[image44]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAFIAAAAUCAYAAAAeLWrqAAABfUlEQVR4Xu2Xr0sEQRTHn6igcIK/UGwqFoPJIIo2gyKCHAabzeAfoMkomA0GixhsVsEkitEmaBNRLAo2q+jn3dzC3i577IC7MyfzgQ8c72bYve/e7MwTCQQCgYANo7iRLJZEG87gIR7hCrY3jPCcSdzGK/zG08avS0FD3MEbHMMBPMNj7IyN8xoNcg3n8E3cBDmN7zgfq43jCy7Fai3BiJgbdxHkvphr6z1E9OAtnoj5x7YMroLswgtJB1nBa7zDvljde1wFGQWWFWSy7j2ugoyumwwsb5B7+GrhJfbXZhaEbZC6JHVOHnUXznrPDeOTpAPLG6R32Aa5LOZ4kscD7DXTUmQFllX3Htsg/4oOPJd0YFGQunPrDp6FjkuugGYOScEHfVdBKrv4KeZMGzGID2I6nWZM4bqFupK6azMLIgpSO4qs91lRTIhpBuLt6QJ+4Gys5jWLYn6Etoc/db/wXszTLosqPuMWbuKjmNa17If6L9DdfbWufg4EAtb8AorQW1MEr+e2AAAAAElFTkSuQmCC>

[image45]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEUAAAAUCAYAAADbX/B7AAABAElEQVR4Xu2WsUoDQRRFXwohAUGDhaTUIsFOENtUNhaGQKxi/iOQVgTxBwQ7P8LKIpZC6iS9Tap0SZNGz2OyMJllYBECC28OHJa9u1O8yy48kUQikdgPNezjG75ga/exPY7wEx/xEC9xhj3/JWsMcYJ1L3vAOZ56mRm0CC3kPcivcYWdIDfBBS4lX8oVrvEpyE2QDR8rJcxNcIe/kh/edCm38v9Sqtgo6AlW3LHyExs+lvtoobrXFPEZj92x8nOOC8kPn5UyCnIT6LL2hR/ifoeMG9xsryYZ4A+ebe/139ft9lvctmuSA3zFMXbFFTIVt+6bRr+OJt5jW1xRiUQiyh+cJzDevoumBAAAAABJRU5ErkJggg==>

[image46]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAFIAAAAUCAYAAAAeLWrqAAABeElEQVR4Xu2XTytFQRjGH0lRykWRsmFBFkpha2Vj4U9Riruw9A2UspKSL6Cs2PkCSllQNsoa66tbVrJho8Tzeu+pMafDGZ07nan51a/bfc+cmvvcM3PmBSKRSCSShw66Ro/oAR39edkrMpcNWrHqpaeLXtBd2kkn6D1dNgc1mR66So/pC63RAXNACGzRW9pt1NbpA+03as1EglykU/QUAQYp4UmIJ1Z9mr7SBavuA5lLcEGO0Wekg5ykb3TPqvsgyCCTwLKCtOs+CDLIefqJdGAhBblDHx08h+7JhTKH/wfZDv2xeeylLXrbn7gGWQqyAsuqm8ifIOfOPO4j/7kwyCCH6RPSgSVBblt1H7gGKWdfewX8Zh9t/b6zQGQSV/QMulQTZul749M3rkGO0xUHZSVJ91Q4VegmPNT4LnuZdDk30K7HNxJknQ7aF8pOGz2kl3QJGuIdtFX0hSy3a2gTIC8/8QMa6KYxrvTIUzgCffRnoOFGIpEg+QL/kVtdLrM5mwAAAABJRU5ErkJggg==>

[image47]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEUAAAAUCAYAAADbX/B7AAAAm0lEQVR4XmNgGAWjYBSMAtoCBSCOQBcciUATiLOAeB8Q/wXihajSIxOAAiUAiK2A+AnDaKCgAEkgfsgwGigoYDRQsIDRQMECRgMFCyA1UDgYIHqIwcJAzAjRNrQAqYHiCcSziMQdQCwA0Ta0AKmBMiLAaKBgAbBAWcowRPM/NYELA6QlC2ri/4fiL0B8CYh1kdSNglEwCkYBPgAAERUiKf4jYx8AAAAASUVORK5CYII=>

[image48]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABMAAAAXCAYAAADpwXTaAAAAnklEQVR4XmNgGAXUAApAHIEuiAaYgTiMAaIWA2gCcRYQ7wPiv0C8EFUaDDiB2BeIJwLxUyD+CsTGKCqgAGRYABBbAfETBtyGeQKxAxDXM+AxDAYkgfghA3bDkEE5w6hhMDBqGAogybClQMyIJocMQIZ9A2JTdAkQcGGApHxQVvoPxV+A+BIQ60LVcAPxCiB+h6QGhJ8BcR9UzSgYNgAAVEc0gKU5iZIAAAAASUVORK5CYII=>

[image49]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAE8AAAAXCAYAAABK6RIcAAABtklEQVR4Xu2XTSgFURiGP6EoIUqJIjsLK7KQpY2FSIqysRHZKBsrKylrS0ks5CdrWd0NC1uWVkhZyYaV/LzvnJmaOWfmmqm5Z7o5Tz3d2zd3mnPfmXPONyIOh8Pxv+mFc3rRMo1wAbZqdds0w1W4CzdgZ/Swoh+uwBL8gofRw1Zog7PwAL7BR0kYrCV64C1chA1wHN7D4fCPCMObgiPwWYoLbxIOwRMpNrw6uAfP/e8BW/BS1Mww4GA56CLCC8PrFxleH3yB61p9Gn7AQa3u4cJTjMFvMcObgD9wXqt7uPAUQUhJ4el1j2oNjzvhUwa5bnGNTYLhxIWUa3jchXhOGtthjTrtT7KGlzdrEh9SruFx+2YPlMZtSd+3FR1eUkhJdY+s4VWKrOE1ifmkl7MD1npnxjMKP8UMKQiPu65BtYY3AGcyyBkT26v5dMEHuKPVl+CrqL7YIAjvSNKvT5WA4bFZ79YPWIL/fRPewBa/Vg/P4LFEG2evr+Fg+WrGx5K+wztRd9UGnEpXoq4bjIHj4biWQ7+zBUO7gKeintR9eC3F3dCqg+si3yY41flZbp10OBwOhyOeX8mHdWKQ+dqvAAAAAElFTkSuQmCC>

[image50]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAAiCAYAAADiWIUQAAACuklEQVR4Xu3czatVVRgH4BUV2BcNHIQgBIGDctKkxJmIgwJxYIpCmIOgBo0KpIEgEjRxZAYZOPADVJJmFjoSnTswKGcNhCII8g8wyt7Xdc4966x7zrlXvJ6O8DzwY+/1ns1d+85e1t5rlwIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADwJno/ci9yP/NOcb28vmqO7pc6fGZ4fGrti7XxYRnP9OTjmXM+1FwEALILdpTYrrRyv62rzsDHyW+SFpnYn8nozXskrkZ/64hST5vqrGQMALIQTkb+7WjZsz3S1edgR+ber/RF5ravN8nHk27K6+580VwYAYKHkitL3zfi9yMlmPE8/lvGG6dXIz814NQ4PjsfGqsvlCuKkufIIALBQcpVpb2RDZH3kqfGfp3q6L6yBvJdTpd5LPtp8dvznB/f2ZVdrbS2j+89HvbPkat5wrkw7163Ijcj1yDdNHQBg7l4s9T2ufHfsYe3vC2sgH8XOarTejpzti412Ve3lUpu+aT4v0+fKpnDY+F1tfwAAmLdsWLJxmeRI5IdSd01+Hfmi1CYmNwC8WerjxINLV4/kStWeGdkyunRMNo3XS20ip9lWZl/zTjc+XWrj1htubpj2d24PjrmKeKn9AQBgXvL9rTNl9DmPfPzXuticn4vsKrVxeSvy0qB+fumKR3egjD6xMeml/2wUPyu14cudnPkIs7evL4TNka+62kpzpWz0hi435wAAC2O4wpQ7LXMFLhuY90vdgTnUnj9u2Xh9Umqj9muZ/JmPTX2h1Ebvjb64gmxm838d+qU5BwBYGJ9GPopcGIzzg7LHI9ciH5T6mPRoWb4p4HF4N/J7ZGepjVquCF4Zu2Lt5Md082/n6uF3kZtl9ZswAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACA/9F/y/hlyKdgLDoAAAAASUVORK5CYII=>

[image51]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAAiCAYAAADiWIUQAAACt0lEQVR4Xu3cTYhOURgH8CMUIR8piQgr+YislKXChpKiWMnXQhYKC5spG5RCPgo7C4msbJTFlJ2FWIhspGSrZGPFeTp3zJk7M9638b4zs/j96p97n/fWmWv19Nxzb0oAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMB09jPnd86tqvaxqUWmwqtU1v5e1c43ta85m6p6r0zFmgAAXVmbcz+Nbs52tM4n28lUGqVlVS3+zm49aBcaB9uFyv+uCQDQF3tyzqUyWVrY1BankU3LZJuXszKVqd+hqn62Ou5kd7uQzU/j31cv1gQA6IuYRK1IpcF5lDMr5/CIKyZfNJFhbs6v5ngiTWTdeMV93a7O23q1JgBAz8V0bUg8Fh3IeVzVOpnRLvRA/TjzQ86aNHYTuSHnTbtYuZiGJ21Xc7ZXv7V1WnNRzqecwSbHq98AAPpqZ3Uc+7eiWblb1TqJJigeNfZSvW9sIOd0zpWqNiSarB/tYmVjGp6qxbUxPRtPN2u+ro6/VccAAH0TLxzMqc6jwYm9bPULBzGhepizNWdXzpamfjTnRs7nNP4+rwP/yL7qura6iYwJXkz+nla1EPvt1uV8adXboqFcnob3542nmzUHm3+XpNLQAQD01alUmpKYFA01YSEatNjvFaLZif1t0fC8zVmVSjMXDc3+5vhFc20vxARr6HMiM6v6szT6rdVoIo+k8gmSTt61C5Vu14z9bHur4/j/AwCYchdSad6icXmfsy2VZmVpzvpUPoNx8+/Vkycaxpj4RSP5pPXbWDa3CxMQ9xsTybAgTc19AwCMsjrnRCqPPGc358dS2Rd2Ked6zrXU+XFjL91J5WO/8RLAmVQeyQ7UF/TB5VQmefdyXuY8H/kzAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAwnfwBzxVoDuB6/QIAAAAASUVORK5CYII=>

[image52]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAHAAAAAYCAYAAAAiR3l8AAADXklEQVR4Xu2YS6hNURjHP6G85RGJ8shEkfIqYkAGJI9kQMwkJaXUpZS6JQMThBEiA4lkRkJSDBQTBpRSl0QRJtfgksf/v7+99l17nbX3Oee27977aP3q3z2tb69zz1r/tb5vrS0SCAQCgUCArIY+QX8tfYM+x597odPQONOhImZB293GNuFYv0j/OO9CI604x3jPilO3oNHWM2XB33IAOg8dhaalw41chH5BK532RaJmcmBjnNhgMw/aBz2EfkNX0uEBMUR0Un5CfdDydDhiK3RT0uaWyUzoBbQHGgGth95Ay+yHbMZCj6G30FQnRtMeQX+gtenQoEMDt0AroA9SjIETRL+Hq5s77JyoqTYHoZ1OW1kME91MXED8bDgujRkjgRP1VRo7EQ74mfh3Z1kwfbyTYgxcCJ0S/c7XogtjthXn+C/Ez1XBHNGSdthpZ1b4AS122iM2ia5GrjwXphimmqfQeCdWFkUayJ21N/7cLTru/UlUZLLo/+HCrQJmOWY718CNor/VmxnOiH+H0bAHooV/iRMrkyINPCn9q3g+9F3Si5NzcDb+XAXGqCwD3fakxnGXXRUt8BQn6yN0GZphHs6BJ6X3bYj5fGLUszlFGWjqH3cZYbq8Jrri18Vt3J3eVe5hhzSOK0/PoblRz2xokM+oTANN/bsvevrhZBnxBFQHijLQ1D/70ELjaCCN5AGhyvpHWMZ8RmUaaOrfETdQI4oy0K5/BqZOplCm0jVSbf0jWUZltUf1r4grAlOxvXubaQo0NOrZnCIM5K5jbVvqBsAu0cl5BZ1wYnkwQ7njyhOvaMOjntmwBvM84hplDORpNMHc/3qg6XZgACyAtrUhXk69dxoPzQzk4pkkjfc5G7f+2XBieaXIPOVlwJLjjitPm6V53acPPaIby4aZg6WOJS/BnMJuS33qnQ9jIA9Zrkk0jm8t+sT/VsXA9MjXYqPcQEy3eCaoAji+Y5I+GXPX3hCt09E9fZXohHDFGfH95+64Q11gWudFm6/RzO/shV6K7njC3XdHtAx0xW02/A72Mf1p9IbUEwoXM0/GVdY/A43jmK6LZqpL0BNp7TbQsdAo+0Le6fB8wPsqUy//tnpe6FgOSX4KDdQY3tv4ArjVQ1GgZvCI/V/Xh0AgEAgUwj9Smc36jMf4ewAAAABJRU5ErkJggg==>

[image53]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAHAAAAAYCAYAAAAiR3l8AAADTElEQVR4Xu2ZTaiNQRzGH6F8y0ckSlciRb6TQpHkIx9h42NncS2UUiilbsmCBUIplCxkIzsSirJRbCywUkgUYXMtUHie83/HnXfOOe+dk3vOeW/mV0+dOzPvec/MM/P/z8wFEolEIpFIiJXUB+q3py/Ux+xzN3WGGuUeaDFDqZ3UReokNTNf3RDq6yf09PMO7Psd6uNdr166SQ332rSa+dSasLAWl6mf1LKgfAHMTHVsRFDXbEZT96hjsHfPo15Q2/xGDTIANhl+UN+ppfnqClupG8ib20qWUIeoJ7BJdDhfXc1I6hH1ipoY1GngHlK/qNX5qqajH65OjPHKdlEvUf07Y9F3XaX2wwbnPMxUnwOw97QLGbiRWk99Q4SBs6jPsFk3KKhThzWItVZnM3Hv1WD7LIaF9U1BeSxzqdPUJNhEeEd1ePXq/6WsXbtZiEgDNRiajZp5IQoxCjWPYSGtVbhJFRroOnU8KI9FK6sz+9wF6/e+v7XAeNg7/VXfLqINPIvaK0yG3Ycl/kVBXbNxP76egWF5LKdg3yFmU1+Rn5wag3PZ53YTZaDLcVpl12AJXtIAvaeuUFNc4wKOUm8bkHaAYytP1kY5QKsjNOpfDHT5T6tMKFxeh+X3tVmZVmds/tuB6n4V6Sk1vfJkHFEGulCl3d5UWG5wGuK1azXr0PcGuvznb1pknAyUkdp1liX/iSgDXf47Ela0mXpG1SuPwc9/DoVOhVCF0lUoT/4TUQYq//XFEUGh2F+9vWkCNbDyZG2mwS4YQqNcpxqdcFp1ym3axYbshk1inTFPBHVFKEKF/SqSjj6DK0/G0auB7vz3mpqcr2qYOdT2BqQQWXRQdrn5FvKhXBNNh3B/wqntOFSf53zC/OejgdWRQibG5j+hlBP2q0ibUZz3Q3o10O3CwkEqC1oZSv4d2d8ySLcy/q5Rxj1D/VsVh8KjrsWGhRUZXbC9gPYEZaFutFlOvUH+zk/3n3v8RiVA4eYC9YDaAjPvOexKzaHVdxuWBg565Q6t1G709FNGb8i1MDSZtTMuQ/7bCzsBhP4oWir19Cu06mbAQtAK1M8hMso/kCf6Gbr8LQqhiRKjc5v+m1K0KUqUGN3axNwWJRKJROL/5g+Z9s5AfA8ZcgAAAABJRU5ErkJggg==>

[image54]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAAiCAYAAADiWIUQAAACuUlEQVR4Xu3cv4tUVxQH8Bs0EE2sUoSAELQLgtgYsQuSQkFSRIWAqIWgYCqFYCcipLMQRS0s1EASInYqppK1t1BQuxRCgiCYP0DR5Jy8nZ07d2d2XNwdR/h84Mu8e95l79vucN+PUgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgPfB6siLyL+RV9XxtnrSBP1TuvUzveMfB2YsnYOlv9az2d9ca1U9CQBgGnxXumalluOPmtokrI38Ffm4qj2JfFmNRznSFsK6yOm2WBm21vNqDAAwFc5GXja1bNhWNrVJ+Cbyuqk9jaxvasN8FvmkqR2NbG9qtWFrZQAApkruKF2vxrsiF6vxJN0qgw3TF5GH1Xic88142K5bT+4gDlsrfwEApkruMu2JfB75NPLB4OmRVrSFJZDXcql015I7Zh8Onv7/2n5qarWtpX/9C+2spdzN662Vqde6H7kbmYlcqOoAABOXtxDzOa58dmyxvm8LSyBvxeYzdaN8FbnaFiv5wkA+t5ba3bbW8TJ6rWwKe43fH/UJAIBJy4YlG5dhTkRulq4JOhc5VbomJl8A2FS624kH5mb35U7V7gWypT91QDaNM2X+c2i1r8v4OZdLt2OWu22j9F5uGPV3Hs/+5i7itfoEAMCk5PNbV0r/cx55+6/2W3X8c+Tb0jUumyNrZuu/zM14e/tK/xMbwx76z0bxWOkavnyTMxuyUfJW6oO2WBm3Vsqmr+dGdQwAMDV6O0z5pmjuwGUDszdyeG7G4PFy2xD5oXSN2p9l/Gc+NraFRchmNv/XnkfVMQDA1MjPYRyK/Do7zg/Knonciewv3W3Sk2X+SwHLYUfk78jO0jVquSN4e2DG0smP6ebfzt3D3yP3ypu/hAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAALxD/wErhGV9RM7WiAAAAABJRU5ErkJggg==>

[image55]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAAiCAYAAADiWIUQAAACxElEQVR4Xu3cP6hXZRgH8Dc0SExSEUIKxaJBMzCaAkehXBIkSMhJLB2iISgHlwsuKhQVaZBCg4OE4uQiNFxoawgdQnERIVoFcXHS5+E9v+57z/3343bu797h84Ev9z3POfD+zp0e3vOeUwoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAALCWPYk8i/zU1O51tcxq+KPUuR81tW+62j+Rd5r6UFZjTgCAsbwRuVTmNmf7e8eTdqLURunVppa/cznu9AsLGHJOAIDBHIx8XerK0itdbUuZ3bRM2sbI66Wu+h1p6l8143FtiPzSL85jyDkBAAZ1OfJa5MPI1cj6yKezrpi8bCJTNltPu/FymsgXIue7cd7fYoaaEwBgcLm6NpKPRacivzW1pWRTNLRsIkfuRnaV+ZvItyN/9YuN02WmUfsh8n5zrm+pOTdH7kemu3zWnAMAWFEHmnHu38pm5eemtpRsgl7uF/+ndt/YVOSLyLmmNpJN1uN+sfFrmXnMm79ztNo2n3Hm/LMZ/9uMAQBWTL5w8FJzvLfUvWztCwe5QnUl8m7kg8i+rn6s1FWrB2XhfV4fL5JDzXV9bROZK3i58ne9qaVsxN6MPOzVR/J8/3HmYo9Fx5lzuvu7tdSGDgBgRZ0stSnJlaJRE5ayqcl9bClXpXJ/2/bI7ciOUpu5bGgOd+Nb3bVDyBWs0edE1jX1G2XuW6vZRB4t9RMk8/muX+h80jsed87cz/ZRM87/HwDAqjtVavOWjcvfkfdKbVa2RXaX+hmMH/+7enKyYcwVv2wkr/XOjezpFzpv9QtjyvvNFcm0qazOfQMAzLEz8nmpjzxf7I6PRy5EzkS+j3xbZvaJTcLFUj/2my8BfFnqI9mp9oIVcLbUlbz8PMjvkZuzTwMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGvJc6W7aFq65vBvAAAAAElFTkSuQmCC>

[image56]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAAiCAYAAADiWIUQAAACF0lEQVR4Xu3cvcuPURgH8KMMRERKIu+LUhSLshAxy2bzOhiUxR+A0WuZTLIYZKKQwaCYLcoiUkJYpCy4rs55Hue5h2fw6Df8fp9PfbvPue4z3OPpvNylAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA4+pY5Gvkd+RTa3+MLOwHjcieyOdSv+Vhqy2JPG61e5FFrQ4AMFHWRJ50/VWRL5EtXW1UlkXOlDpBm9fVj3RtAICJsy9ysetvjHxoz1HbVuqE8VVkQ6vNb3UAgIm0IPKgzJycvY6s6/qjdLk9t0ZetPbu9gQAmEi5uvar1FWtzMqZr2fVb1n+D7mKdqXr53flWbqbXW3K/sizYREAYBydK/W82L/YFVk8LM5BnlM71fVzhW1v5FZXm5Jn7t4OiwAA4+h95OmwWOrq2cHI7dY+ENne3h2NXIu8iZxttaHDs2R5N66XE7MVXT9X+3IyObxwsD6yM/JuUAcAGDv5G4+cEGXylxq9l5HVpW6T5vbj2lLPkuXk7VBrP5oePXffS/2On4N6/t4jb472cov0ROTboA4AMFHulno7MydmOXHbUerEKVfA8ncfuXV5fXr06OSZtk2RzZHng3cAABPlfuRk+bvlmbdGj0duRM5HrkYuRZa296NwJ/Kj1JutF0r9ye7pGSMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICx8Ad39UJClYWduQAAAABJRU5ErkJggg==>

[image57]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAAiCAYAAADiWIUQAAACFElEQVR4Xu3cy8tNURjA4SUGLkkuJZH7hAwIMwaKZCp/gcuIETEwUpiYYchMQhlSipIoxv4CJAYGJjIgl/dtr+Ocs3yJTr6vznme+nX2XmfXnq7W2nuXAgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAjKsj0afoRzSrjq2q59nlOjadlkVPSnf/L9Hb6Fu0efAiAIBJsj66FJ0YGLsa7Ro4nwk5YWvPrzRjAAAT4UC0JXoRLapjd6Llv66YfnOj981YTtguNmMAAGNvQXS9Hn+PbtXjk/V3puwtw5OzM9Gp0t+2BQCYGLm6droe55ZormItLv+2ujY/mt0OjiBX1+5Hu6MVtda+6Fk7CAAwjs6VbjUrrSvdA/57ojm9C/7Cg2h7OziCldGraGEzPuhR9LodBAAYR9dKt6LVk8+yfR04zy3IG7U83h9tLd2bpIdLtzr3pkz9Numa6NAfWtK/dMjZ8vsLB4PWRjtKd18AgLGWn8zIidG7Zrz3HFt6WX9zWzK3IVeX7u3Rm9HB0m2f7qzXjGpp9LD0PymS95hKTjKPRR/bPwAAJtHd+puTtNyqzK3PnKQ9jjbV8rtp02VetCHaGD1v/gMAmEj3Srfd2XtrNLc5j0ZPo/OlW3XLLcxt9f//6Xb0uXRbuBeiD9HxoSsAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAICx8BOxVUOvcgpU1wAAAABJRU5ErkJggg==>

[image58]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA0AAAAYCAYAAAAh8HdUAAABE0lEQVR4Xu3SIUtDYRTG8TNUcCgTFJsWsQxsCrJmEpNFwYFrC4LBNBFlVUQwyAzLaxaTXcSoyWARBAd+AcPi1P+zs+G9h9tMAx/4wTjnfe/Ozp3Z0GYSa1jHVL82g9nBgWTGUMcbDrCPJ1zgAcXfo55RNHGNiURd3/CIe/MJUimhjaXYICdoxKJyig/MxQY5xGYsKi184xgjobeI6VDrpWx+Sbq4QxWF5KEYbe7M/MLgsjxb9sipaDSt9hyf5hf3UifMD2nmXGyQDXzhKDYWcGX+nmKW0cFubGiVtxiPDVLBK+ZjQ+9HT1sNdY2sJWyFeu9vcYMaXvqfteZLvGPHMn5r3vyJila+gm3zf3jWuP/5U34AsNUreE1r6AoAAAAASUVORK5CYII=>

[image59]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACIAAAAYCAYAAACfpi8JAAAB70lEQVR4Xu3WQUgUURzH8b+UUJaUBKKQJFmHMojwIAiCRgc9CJHRSfAQZNDJOqgh4iXwIqIoQYVUIhEoCFKCJ9HQoIuH6FAHC8TuIR0Kyu+P98adfbpt4o4U+IMPO/v+Mztv3rx5s2b7+U9zFHW4jnM44NuP4KTfTjTnsYR1vMRdPMcsKjGDK5t7J5B8dOMHOnE4vWy1+IZVS3BE1ImH+InmoBblEF552k4kt/EbXcgLavE8w/2wMVc5gzV8QllQC/PEEpwfveZG40HQvl2OmbuNOY8e0Tn8sgSv9G9Sii/4itNBbU8TdUS0/adoQteEjTtMIe6EjUoR3ln2jpzAKIrDwg5zAQNhY5R+c3OkISz46HHW6hpfX7TYtWEQj82txspNTJkbvePow4S5C6jGe6zgERr9MZs5hY+YR0lQ0wl70G6p9eUgRtDiv2u0XuOS32ccHb52Fm8tNdpN5taijCnHIr7jKVrNXe0CLlv6IqdJ/QFVsTb9ePT4azvqiDow5z+VrB1RdLJyXPUqLPXGjeciPtvWjgzFtrN1RKOq0dpVtPboTRzNKd2+aVzz3+MdUWeXbWtH9BuaR7uO/p9M4haGzb2johVXJ3uBG+aeEL2xx8xNXh33BvdQ7/fPSTIt+WpTTbe2YJua/lzt59/PBpzqT9uDX5JdAAAAAElFTkSuQmCC>

[image60]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEQAAAAXCAYAAACyCenrAAABbklEQVR4Xu2WwStFQRTGj6LIK6KUIrZEFmRhrVBCPcXSgpKdvf+AjWQjioWys7Wy4D+wslX2Nrbi+951axxzu3Pfu3ekmV/9NufMvDl93fvuiEQikUi5jMJNXfRMF9yCvarujTG4B+/hB7z62fZCH9yAl/ANvsBBc4FPGMganIOv0logU/BIFx1gIKtwBt7IHweSwgE4SCuBTMMTXSwIz68ykGU4qYs2QglkEe7roo1QAqnBczisG5pQAiGz8Bh264ZJkUDaYL8ke0wX4IWlTjsbO/MpGgh/V5/l4jZ8gOOSARe5BjIAD+GZ8hY+W+p0qbEzn6KBrMvvs1y8luQTfwdHxEKRQLL4L69MjySB8Inm024llEAYwAGs64YmDYTJZaaWQ1mB8II4pBslMQFPYbtupMxLMgCv7Z/fvsMncbzAGDQbCP+THiU5N52B83CuXWNdGezAFV2simYD8Qk/tR26WBV87Xg1jkQikeD4AjCbWXqBmlY5AAAAAElFTkSuQmCC>

[image61]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEQAAAAXCAYAAACyCenrAAAB3klEQVR4Xu2XPyhFURzHf0IpJBEZ5E8WIoMiYlPIn4TBZsFgs7BYTSyIQSSDTZmUSf6MNkUWSVkoWazi+3XerfPOPTf33XvffYP7qU+9fufee+75vnM654okJCQkREs9nDaLMZEHu+Am3IHDMD/tiphohgvwHH7Bw/TmWGAYS/ASNsAKeAR3YaF2XSwwkHHYA18kXCDtcN0s+qADvsJerdYIn+GgVouVGlEvECYQDmzLLPpgVVTffAeHUngND0TNoKgYgW1m0UauAimCp+IOpARewBtYrtXDwhm3aBZt5CoQZ+BegZj1sPC5e7DWbDDJVSBOv+bAsxUI6YQbsNhs0MkkEK5p7gS8R3cA7lvqlEvDRjV8FPfA/QbC55p9+XEWXsEW8YAX+Q2kCq6J2hZ1T+CDpU6Hfu904zVwr7rJlLj78iO39Q94BuvEQiaBeBFkyRTAY3EP3AmEOw13nCgpExUIZzRnu5VcBUKW4buoM5FDJbwTdXKNEgawAifNBhMnECbnmdofBA2kSdShUP9s6INvsFurRUEr3BY1M630i3oZHtu/U37CW/F5gNEIGgiZgE9wHs7Ae1GfFEH/HC/m4JhZzBZhAiHcuUZT8nc24FYb2/cRlx2PxgkJCQn/jh/ybWglKqQwNwAAAABJRU5ErkJggg==>

[image62]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGIAAAAXCAYAAADwSpp8AAACu0lEQVR4Xu2ZX2hOYRzHf4sVIURDiNVuhFyIi0VJJGEtaq3sys0kJdNyabWUssm/1YhEaUvK1eJqlAvKxUpWK7lQ5tIV7sT3u+c99r6/nT/P8TznnOk9n/rU2/M75z3nPd/z/DnnFSkpKfk/aITLdWNGLIArYYMu1DsM4A7crQsZwQAuwPOVz/OCzbBTN+bIQjgET+uCB3jnd4j5jRr2wEfwhC7kyRZ4Bo7DX/BhbTlXDsJX4m9YWgyPwRtwGv6AO2u2mGUbfAM36kJeMIh22Aq/iFsQO+CAbrSEF+0FPKsLDvA7D8N98JLEB8HeOAL7VHvurIOfxS0I/shbutES7vtRzI2RBRclPghyEr4TM3n7hMdkb7ei6CC64Wu4TBc8YRMEezSvwS5dcITD3qBYLgaKDoLHdTl2EjZBBNfguC44wmGPQcQd+y9FBrFUzCR9WbX7xCaI4Dy4rW82wXtwrS5o0gTBLrZKzD7VHoL3Q9rpopk9w0m6ANxXf1+UPK+wISBNEEk3BJe7a2TusZM8At/C/RJ+jjNwQ9sgmuBVeFf5DE6FtFOuXqJICoL76u+L8gpcYXarIU0QvHPjaJW5x7WRN+lXOCEx55EmiChch6aoIHyQJogszoO96Do8JebhMpIig+Bk9lSS70QXbILgspXL1x5d8ACXxr0SMyQFBEE8FouNI/jXIMhNOCbxc4kLDOKnxC9NN8BP8IAuOML55IEkvDHgQflEzdcbvyt+h+/h9qrtbHAJog1+gKt1wYElcBR+k9nfRjlOX6vaLmAPnITrdcERXmO+VMwNlyCaxUz0vBhF0SfmNQeHSp+wl2fV00Ph8HZUN1rC4bAf3q58zhsue/nic68u1CMco1/CrbqQA11wWMzqpkTMH0JPJGFi80wLfC7m6bekCj55ntONGcGXjHySZm8sqWf+AHZwjSXNyvBdAAAAAElFTkSuQmCC>

[image63]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEQAAAAXCAYAAACyCenrAAACQElEQVR4Xu2XP0gcQRTGnxhB0SBBUSxEEkSJGBRCEgS1CpjgHySmEBVsjIWdjUJIF9LENComIEpIYSdYCUIKE20EazWNiBIQBbHRxka/j3FknN31duf2rnF/8EN5u3d777vZnXciCQkJCfFQAPvgLPwKa28fzgo58BWcgt9hO8y9dUaWKIa/4WdYBBvhNuwxT8owDGMM/oWPYQlcEPUF5RnnZYVxuAkfGbV+uAPLjVoYGuA3uxiC5/AINhu1J3AfvjFqGYchMIxfVv0FPINdVj0VbGzaLobgi6jmK4zaQ7gOf4paQXHRAZ/ZRc1TeCLeQNjYuagPGgWXQPLhsngD4e37R7yrN1244kbtokY3HhSIXU+FSyC68aBA7Hq68H3nYKV9gHTCS/E2ns1A2CybthvPVCDkJZyEhfaBt+IWCO9p7gT8oKZtcN6nTnlr+MEH9654Gw8bCN/XvlYYh+AarBODoMaD6poyOCFqWzRdgv986pTh+xHUeFDd5r14rxVGbuuncAVWyTXc2g7F27gO5KNVT4XLLfMALoq3cR0IdxruOHHC2YuBcEVztd+gL8qnvLmkX8OL679RcAmEcBbibsddT1MKt0RNrnHCAD7JHYPnADwQNSESvoBT64aoJKPgGkg1/A97jVoLPIZNRi0O6uGMqJXpC0fjH3AVdosKg98MR/iouAZC3sE9OAwHRf18GJF4hzLyQUIMnLxojagHVKu4/35IJxDCnYujAOX/mYBbrWt/keFDkaNxQkJCwr3jCiUEdtqi33jCAAAAAElFTkSuQmCC>

[image64]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAIAAAAAXCAYAAADDXUcwAAAD0ElEQVR4Xu2aW6gNURjHP7lE7h2REEceCFEiRHlwLSQkxSunvLnXeXE8KLfkViQSJUShXF7EKUK8KRF5IJe8KMKb+P9bM+fy7b3WXjOz1uyX/at/o/nm7LVmrf+s+b41RBo0aNBAMwDqq09Gosy2fOgJDYV66EAEymzLm+nQeWiwDkRiLHQ9Odab3tA+aLUORIITvx3amvy77oyGHkKTdSAA46D1+mTCfOimlGc6G5yMIxJ+MvikrxMzBhqa7iK0RgfKhjd9FGpT54swCdoCPYD+Qhe6hztg2yeh3TpQIjT9C6hZB3LSD1oBHYM+Q7+hGd2u6GQK9BQaowNlwk68SY6hoAFWQXOhT2I3AJkDvZb8E3AYmqZPepIakAr19NMAy6AF0B5xG6AXdFnCPnyZ4dN3R+IkZCOhD+I2AJMhPoEbdMCTE2If4FqMgt5CC3UgEBxblwEI75v3z3EIyRBosxiTWeGkc/JbdSAQPgYgZ6FLku8pLGIATvx7MTlQDHwMwNWLYzRTBwrClei0GJNbSSeI76wY+BqAA/UIGqgDHhQxANttF1OSxsDHAOkYxahA+JusNKywY9+geToQCF8D0IC8jtdnpYgB2C+Wos5lsgA+BqD52iVOIsxK44A4XnHs2MfkqOFy3CRmUnxULYfIYoCv0Hgd6AJ/X7dJnYOWVDnPvtd6pbBftr7Z2qsmW1tZDMB9CBdZ5yPVROgutEvMa6EbLgMwidgPnfEUM18NO+BrgJ/izubXSmWbFCuYG1XOs++8BxcuA/B+9G/aZGsriwGYB7kYDh2SyrZ99AT6AW0SszfRgcsAIchigHq9Amr1rQhZDBDjFUCYB9AEg3SAcMn9ItWf3hD4GoClELPxETrgQREDcNm9D/XXgUD4GCAtg7fpQAAmiBl7607rMOgV1KIDgUgNUKvEa5X8exFFDMD75v1zHGJAA/wRd4nHEpTmtyZqOeF4HxSz0WaFF/ED0HEdKAhvhjuA3Ab+l+gX9BKa2uU6wgycmXjeJbCIAfh378TsXIaCq8kV6Lt03jvFlZbfGzSswGhCZ72eA5r6lHiUuPxQ81zC70T50ixmK9jpVAdFDMCl8Znk34UMQZuY7eDQpSiTPa99lSboMbRUB0piI3RVTM2ah+WSL3lMYfsskypKpBLg2PODGb+K1pWV0DUpfxA4ABz8WTpQIlwib0OLdKAEaD5u1+Y1fzCYC+xM5ErWQsJ22sR8iy+rTRv8Tym3kmNZMEO/J+W26YQu3AHN1oFILBbztarek5/CRHAv1EcHIsB3M0vQWB+hGjRw8x8n4MDeR3xl3AAAAABJRU5ErkJggg==>

[image65]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEoAAAAYCAYAAABdlmuNAAAD2klEQVR4Xu2YW6hNURSGf0W5XyISZR+UXAq5l0uESDyICHnDAyHX4mU/8ODBJeQBOUlCJC8kFKII8eTyIpdEkpSkKJfx7zGnPfdcc66199n7HHXaX/3tddZYlznHHGOsMQ9Qp05L0FnU3j/Zyql4zqNEjaJuvqGVM0B0wfxm0l90UzTcNwi9RLdFv0R/jH6K3plzFO3D7A01ZrroE4rvvirq4Ni7iq45duqiqJNzTRZTRJeQESRtRAdEee+8z1zoIHZ75znQy6IvotGerVZwjEehC/RDNKnUXGAhNDJcJ5YLn39YtN03uIwQvTC/aeyAOooO8+ELaDvoG2pED9FJ0QboezgpTs5lk2i5d64S6PznogbfYOEkGRFpBY02XvNBNNCzEUZZKNpqxUjRflFf6GSY9u6E2oqOmeuaChfjISLOtg5gtKTRT/RadAPJ3Gde30c8JWoBB7/GHOehi7Lun1XrKCOOk62G46LTSEZrYYXeiOb7Bo+Zot9IRgwfuM3YNpu/m4N9ojHmmCWC9ZCLY4vvZNEhc1wNzK47oi6+gS//CH1RGrYG8evComr1RPRINBVxJw1G0wqsxdYnRg1hmp2BLs4cc47RFkwZB45vCPT+GAwYBg4DqAQ66q35jWHTk6vIzygfYtXduS4E243HCNe1crH1yV0IOoiOosO4COXUp1miu0hPTzoqWIfLcZStT8GQTKEdNOe/Iv35Wbj1yWLrIhdvBrLrE69nfQ1GiwMdxfEmnF6Oo2x9qvTTz/v4yX6G7NSOwShi7RnnG4QV0HLA5+/xbD4rURxLIlocoqnHm94j3BtZbP+0wDekwBXkfYMQ/1gw4nqb3xh+fXLpA20VOLa0+pQTbRSNRXZQ8Dkvoc8ugQN4imRoW5j/3DJ8Fg31bGksE42HbjhvITyRvdBJ5r3zLkwrbkc6+gZDHuljY0TSSTlolGRFNxc32FPyQY1IplVP0XXolsHun3jMHiM2aEtOdEq0GJoe7PpDWwNOgCnN2kGHujBtv6H4bvZo80quUNgqcCFj9WkCtNAvEq2C1rRQdBN+DbkFCo21wFLRA8RfVgl2Bbnns1/G80guhIURzRrkN7G1gNmwE5r+HAfblHuIZ08DNJWjTTOjh59N25NUA8OahdOFNYYKwUGxYW0OlkAj02LLQCxiGP3nkF4zC4WaK9/UxpD91BFoujDt+DdDeb3oFbQ3yaM0cjjwEwj/a6cacqKzou/QnQTrDd+1C/rvGpaCtShtPBksV6B1NRWmzFYjHrcELMDT/JP/Ac43jwq2YAy5LaKJvqGVM1u0GmU6qU6d6vgLMVu+V3NRacoAAAAASUVORK5CYII=>

[image66]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEwAAAAYCAYAAABQiBvKAAAEBElEQVR4Xu2YXaiOWRTHl1A+5oMhmpjGSFOMQlIUE3JBGk3OBTVufVwomfFRSp0bxQ1CEVMnU3JhYi58NTTOiSKmZpRBSoZEk+RmLlBmrN9Zzz7vftezn+e873teH3XOr/69z9lrv++z99prr7X3Eemjj3fBB6pBvrEX0F81XNXPG8qYqmpTfewNvQAc9YNqffbcLWNVF1RfeYMyUtWheqX6P9NL1cOsDWGfFL7wBpineiKV959VDY7sH6l+jezouGpo1Kc7Bqp+UrV4gweP7la1unbPIrGBbHPtDPaU6plqmrM1E8Z5UGyxXqhmVZs7War6WaqdWQ+TVZdVn3lDDJ1uZ59lbBFzGI7zbBaz7fGGJkKOOaxaJ/aufZLfPt+rvnNt9TBAdVS6CR4mS4SUJXts9HmsGu9sQNSloq+ZTFHtUn2quiWWEr6I7Ez2UNavJ+Dwa2ILlCM4gugpY4zqb9V5yecFisQVKd4mzYKJrM6eW8UWaG2X1XItEZicaB3g8PuqGd4ArBbGb7zBsUD1n+QjiC2xKbNRZfwWaSY7VdOzZ9IHOZOFClV9tmpv9twTgk/IhzkYwD9iLysj5CgqEYk36A/V76qv5c06K+QvoghCrmGhFmZtRF9P8leAs2i72Jxz4LAH2WcRYduyonPEViBoWNQP+JvSfEf1p9iLEZGwUuyA2Aghf8WLgqNwGI6jKqby17eqq6q7qotiY+GTSjquq1c1wWF+N3VSi8NC/uJFH1abkhANDHJJ1MYWeqSaG7XVQ5y/AiF3spDzpTh/8d12MUcATqfCnpB0oQsO+9G1d1KLw0L+qvXIMFEsuuJqyu/jsEaKAhMkN6WS8AqxVHFTtcPZAow7jhZ+r00sMtnantItyaSYSOpsFQjnrzhiyvAr+onqjFhF83mOPiMS7TE+f8WMFjtiML5U/vLRznuWie0Wbjcp+A7HCs50ORjEX5IP9wC5gWvIU7HIqQVWtEO1Vaz88/1Vks9fOOq6dH8cYbtxzRniDRmtUjw+2u6JVVjGw9h+UX0ed3LgSHIeOytHCE+/3ZjMObFrSLib8XxEigcOfkXjtjVRGxBdp8W2+0ZnAwb8r1Tej2MXV/UwyI8sSi35K7TdUI2K2mI4MRBE5O4ky8UmlHphvaTyF4WCLeAXJYBj4gNoM/H5CzhfFUUktEpxfuuEaLoklfNMT0itaIvYlaroYs7Bt2xLNkoq2qms3Fb2S9oh+OI3seNTKfzoMWn8ls/5a7vYJZ6cEQ62DO6k6stK1yo4N1G+G31vERQxzlrPxY4PjIXCQfQTzfwrJwVV94AU27sgl5FHUFnFajZcyYqq1dtmglg1LysIVeDVDaqZ3tALIM+S696Xxeuj1/EaR23OmwRTHJkAAAAASUVORK5CYII=>

[image67]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAAjCAYAAAApBFa1AAAC4klEQVR4Xu3cT6hUVRwH8CMpFEoWQSkaWggRtClXQm7EhbZwJZQItWhXm2jRH7A2FUSujBaRorQIRNsGUS4eES3SrSGJUG2CIgIXgQjV7+eZoTPHO8N74jwn+Hzgy7v3d+68O7P7cc65txQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOD/aEPkYOSu0fnWZgwAgDtkXeRI5I3IPaPa7sjVsroN2z/N8b2RPyNPNrXlWN8XZrjeHD8V+a3UhhUAYOFko/RmXwyf9oU5ujvya1fL7/VeV5vlwcjpyK5+YMDjkc+b8/sj5yNPNzUAgIWwI3I58nA/EE70hTnaEjnXnG+MXCvLa77Gnou8FPm4HxhwIPJqc573yfvlfQEAFspSZG9fvEWHIr9MyYVSm8NpXo98FflklLx+zcQVpTzWnfeOlvqZfaO/0+Sy51Lks1LvlTOJp8rqLv8CACzbz5FH+2LnvsiPpTY5mf1ldkO0Urkc+kWp++Y2jzLk277QWBt5YnScDdkjzVgvl0P/iGwrN98vP3cl8n2pv/W7cnt/KwDAiuW+raEG6WSpe8LGsmEby4cBhvZ6ZeM1boD6PFTqww1Dcobv777YeT7yQ18cyYbqg652pkx+/9aHZfb9sknLPW0pl05nNX8AAHOXs0zfRDY1tbfLzbNKec3hUpcRV/Ik5nJ8WeqM1zSvRLaX6Q1b7lvL/WutbLKm7cHLhjNn9IZk03k28lape+qemRwGALgzckbpr8gLkWORPZPDN2abslm73R6IfF3q06CZbAaHHC/13XDZaA35qNT/1cqG892ulkuuuQQ8vt+Lk8M35PLwzub4p/+GAAAWVzZr4yXC1fZsc7zUHM9LvkZkPLv4WuRiMwYAsJDej1yKvNMPrIJ8p1rO/OUyZT5I8Hvk5VIfMJiH7aW+C278pGrO7LVLxQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAwAL7F4kpYB6HT+dpAAAAAElFTkSuQmCC>

[image68]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAAjCAYAAAApBFa1AAAC1klEQVR4Xu3cQahUVRgH8BMWJAYRRhq4eLQJIXDhIgIDkYJSbOEqqG0otDFBpFYPylVoRAVhotZGJWpjQcsoiBZtyyiElMiVuNaF9X3cubwzJ+e9ebw78xR+P/hz7z3nMufO7uObc6cUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADgfvFIZHdke2RDZNPYLAAA6+ahyO3Ixmrs+cjf1fU8nCrdczxXjR0o48+1GtMWnLnurTLcugAAgzoU+TfyQDsR3mkHZuyDyJNlvFD8rDpfjRcjn7eDE+S6l8sw6wIADO6fyJ/t4MgL7cCMHRwds4DsTVt01R6MfBI52k5MkOsulrWvCwAwuNyzdqcMW5hdWybfVfe1Hos8Pjo/H3lpdP7a6FjLbmAWZZOcLF2HLe/L43L6dfPzVloXAGDu8ufH65Gn2onK5sjFyKXIo5GnI79HvqhvGsCOsvSzbBZNWTzlHrIcb2URloXWJKfL0v6zT8vyxd00634TuRL5PvJT5OVqDgBgprLouVq6wq2WRdqZ6npn6QqZTH89SX7WpDxR3VfLgumjZix/nvytGesdKZOLzC2le/5eFmHvV9e1adfNDmQWaumV0nULAQDm5o/ID9V1dpfeKktdpzzmdXbXfi5dkXe3FxTWYk/k62ZsMXKjGct1D5eu+NvVzKV89uyutV5tB0amXffbyJeRdyN7y/DfHwBgWQul6x6di3wY+bGMFyS5v6vfvP96ZF81N4TsXmVXK1N/9jPl/3veni3d25tvRPY3cyn/luPtdrB0nb0sOGurWTc7bn1X8a+y8r44AIC5yu5UvZ/r1+p83vqXI/JliWP1xIydLV0Rm8nvv218GgBg/eResJuRr6qxE9X5vCxELkSORx6OvFe6Fx/erO6ZhYXIx5FfSvfnuvmT8Nb6BgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA4N7zHzS+YwJj6tUpAAAAAElFTkSuQmCC>

[image69]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAAjCAYAAAApBFa1AAADlklEQVR4Xu3cS6h2UxgH8KUocsktEhIDkgFyKSWlDBhIYUAYGSgpRpSJb0AGSiGXXCIllwyUpGTgMkAMMKBk8CUlJFOUy/P/1t7nXef1Hk6n9/s6g9+v/p21L+esvffo6dlrn9YAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAANi2wys/DNv3V/4ctnezXPu7lTOm7YMqv1ee3Dhj/8ic8zPLnNe0PufBG2cAAKzRFZUHhu1rK39XDh327Va59r+W9qWQGgvQ/5Ji6+Vp/FjlkMrNlVM2zlgtc47PLAVj5pwLRwCAtbqn9cJnlgJmucN2VeXUFfuOm8bpdKXYOapy2cYZ+1+uPcXlKNtzEfZ/rmv9uiMF2+y1YbxK5sj9z25ofU4dNgBg7U6u7K28X/mu9c5Riq/Z05UPpnG6R8+3XqQ9W7m38v107I/Wi76LK79O+7brs9bn3io3Lk79l72Vn1u/zlcrP7ZeNI6OX9qepcga//ZYsOW5jM9hlGOZM88i82bOhzadAQCwRuMrxXSaUoScszi8r9OWwibOrdxWubD1QufjtuhkpXDLa8S8YszfiLsqn7deDL5XuW/av0659mcqJ1VObItu2eywyptL+2YpONMly+8mzw3j06efq+SZzXMm45y5529av9+PKscOxwAAduSttnm9V9Zl3TSNU4zk+LyWLV21sVs1r9lKoZbiJ1LoPDKN5w8CZp8O49EJbVH8rMoRi1M3yXX9Ujl7+cBgT+WV5Z2TO9rmV8Fjh+3ItrozlznzTLaaM8fn55fr3rM4BACwM+Pi+aMrn1Subn0t1vmtd8vSNbu79XVbl0znRgqwY1pfpD8XKU+0xbq2dK9SUF1febBy5rR/Xa6sPLq8c5BOYa4969xWrS1LZ+ypYXss2Mb9o8y5/JHDaP6A44XWC1wAgB27pfJT60VYckFbFGZZj5VXe9n+svJi5c7WC5A38suTLyofVh5v/ZVoXimeNxxP4fL6sL1O47Vv9UXow60Xiy+1rbt0p1UumsZzwZbO4O3TeJRnNs55+ebD++Se89zmcV7JAgDsWt9WLl3eeYCkyzcXS+kYbrUebZa1efmAIF+4Lq+D2668Hv5qGqcQ/Ho4BgCw69xa+a317tyBluIrxWK6fSnA3mn944izxpP2g7dbv+cUfvnyNf8yBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA2AX+ARzYje90twQqAAAAAElFTkSuQmCC>

[image70]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAAjCAYAAAApBFa1AAAD30lEQVR4Xu3cT6htUxwH8CWUvyEvf0IiKaUoJq9kIAnJwJ+R/CmJZCCSqFdXSAYIhVD+lCRmKMngyQQZSEiZPBIDA2WE8md9W3vds95+53Hdd9+7b/D51K+99tr73LPPGX37rXVuKQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA/KvDa31Q6/Tp/NBaf9a6evWO/d95tf6q9fowd2Sti4bzveWIWu/X+mE2f3+tA2dzAADrcnFpYWf0d613Z3P7s8dr3VHac3cXDOO1eqW0wHpiaUHs2FqP7HTHri6rdU+tX2odNc0dU+uN1TsAAPbQvWXnwHZQaR22K4e542udMpxHzhNounTq4sLSQs++knCUoHVaaV2uPGvcunrH2hxc64pp3ANbJIwdMI2XWSkt9Ka7d+k0d1atZ/sNAAB7aketX2ttr/V9rfuGawkxz5fWOcqS6U+lhZcEtVQC0smlBZY/ptek03TdNP4/8t67q8+G++byXj2cba31Xq0tpYW4ZU6YT5QWUp8YzsfAFncO47kXah1S2neUz352rUfL+jp8AABLpbv28DROQPt5uHZVad22OKe0YBfnlxZQ0lVK2Mnr+x6ul0oLcJF9XN9M48Om8UfT+UbJcmj2sEU6e7+XFpaeXr1jIdeXhcl06Z4rLail8lnPGM6fXNy6i3TguizJrpS2HNo7fQl7n9f6sLRQ/Mw0DwCwJukMpWvWf3AQ4z6w78piL9uLtb4armUptb8uYS3hLBJuEuIiXarrS1uujBun4zI9HC2rHn6WSQAalyw/rfVJacuSo+NKC1M9nI6ynDsGvHmH7aFhPMo96TB2D5b2/Y3LsfmOx/2A24cxAMB/yp6rMcAcXVrgSOBKgMnyYjpmCUSZT0jrm/CfKq0zFbmWzlVfKu0umY4fl9aRO2m4thHSMbt8NpdwmGebu6W0z/FWWQTKUTpq/UcDY2DL/Bjeuvytm6Zjl7+brmOWZLt0GxN8t9V6ubTvGABgTdL5StBKfTvN9WCWZbwEuXNrfVFacEuHLXvJTp3uzTHLm2/Weqy0Lts707Xurum4o9a1pXWbNkq6Yv353x7m8xnmy55Z6r2t1jWldbiWBbB4dTqOgS2ffZns2ct7/1ja99QlBI+BMN9jQm83PisAwKbKkuQN03hrWex/2wwrwzjdrgSy3cl+uAdq3V7rzNm19fi6LPbYJeTePVwDANg06XJ9WRY/OMjSZTb1b4bsJ+vPkX85kh9RjP9gd2+6udZvtV4r7YcI6VCOS6gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMA+9g92lo4YIlhXZwAAAABJRU5ErkJggg==>

[image71]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAF4AAAAXCAYAAACChfjKAAAB00lEQVR4Xu2YzStEURjGXwtFKV9FdsOCLERJysKGkkKKhfAX+AeUspISdhZKWVhYWymlsFR2yseWlJUdGyWe95y5051zz9Fc9xxzb/f86tc078y5PT1z7505Q+TxeDyePFGAC+owQxQoHfnr4SI8gNuwp/xlSS9cgRfwCx6Vv5x60pa/EZ7DDdgAB+A9nAu/ieHgs3AEvlCy4P1wVx06xmZ+G6zCG9gcmi3BB9gempXogE+ULPgg3FOH/4SN/Enhsrl0NcMQfIczylxgI3jei+er742iGbiXD7ipzAU2gue9+KBgNYNpLrARPEvFt8Az+BzDdbHSzDT8pmgGa8XXwFaSa8JOwEPNnK0TK/WYjmdSdyyeV5rfFZPkuPg2uEPyd2rYE/iombMcykQT3KLoGpO6Y8XJ7wpTwaa5wEbwLN1q4l5lLJ8gv9EFXymaISh+TZkL4gbXkaXia+EYnI/hsFhphjdMV/CUym+H4/Cz+BghCH5M8mz4C2koPkl+GyyT/CLuLD7nLLyLvSa5qy3BnwLv+Hi7zV8MLP/Yv4V9ofdVQjWKt5nfBnwl7cNLkjtqLv2O5F8HzqhG8WmEz/JukrenUZIfhlP4cp9Shx6Px+Px5IUfaaCHWS+Qt8AAAAAASUVORK5CYII=>

[image72]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAmwAAAAjCAYAAAApBFa1AAAGCElEQVR4Xu3cW6htUxzH8SGXXJPIJeSSF1HkVgpJlFNIKIRSHpROKUK51JIkFBERSpRrinINseOB8KKIBxIdFMmLB5dcxteYwxr7v+dae+291zqdtX0/9W/PNdY6e8415qr5O/8x105JkiRJkiRJkiRJkiRJkiRJkiRJkiRJkiRJkiRJkiRJkiRJkiRJkiRJkqSxdo0Dc2S3ODAF/M6t4uCcmMV8SJI0d7bNdViuk3Ntn8qFfev2BTPEfnaOg2tEWDsuDs6Rq9N0wxXn95w4OEemPR+SJM2dH3Pt1zzePdffzeNZuyjXsXFwjRbiQHZErudybdM93iHXn8OnJ7Z/KgFoOQSM33P9FsY5Bva9nPdS2Ve0Uxzo7JEWn8fWh3EglZDM8W0K47el1YX1vmMd5Y9cJ3TbR+X6IS0f2kfNhyRJ6xodNcJa39Lhu3FgRljqIkzUEDUNR+c6NA5m96ZhSKgIpivd94O57o+DPQiI++T6LNdB3Rj7YnwSBNm+oIW+bhnH1BcE9811ahzMNuQ6PNfPafgZ4Hzs9d8rVoZ5mWQuOTdtcK6fgXhuonHzIUnSuvVsrr/iYIdw0+Ii3nY36MBwgeXngbnO7rZbdKEYb9F1al/Lxfun4dNTcXmuXeJgKl23NowQGFbaSSTY0MW6Jz7Rg4CBQa6N3TZdsEnvxyLYfR0HO4+kpeHsgvC4Iqz1dd4GqczBU7lO78Y4H5OErqjOy/HxiR5n5bqqecy/oQvZ9x+H1rj5kCRp3SKsxGAWEbreSeVienCuR1MJXQSWPVPpzlQLqSxr8TydO+4hI0TUffyS6/xu+5lUlvZ47tNuLHot1zdjqg/7X4iDHd7vQ10t5PoyLQ2ZBKpRCEiPNdt0KMepryUAEYwJRYTJSY17L4QcQls1KqyB4+gLYQ93Pzm3nEe6bbcPn16kdgj7tPPCz3HzUt/TE6mcB17PZ6ovUEbj5kOSpHWLAFO7QKOw9FbDAB2OGjguTOUiWy/wfFHhzVRCGEuSg26ckLd3KuHu/VTCwSG5zu2eZ4mL5bFpGXdRJzSxRElxn168iZ3g8WIYaxGS7mge39Bs97m72WbfdLJqSAKdxg9SWX6m4jyMey8EsLosy3G34S2qYSq6ptnmszBIJUhHzNOtcbDRzgufl3HzUjuqB6ThuajWMh+SJK1bXKTPDGNcnG/strmYsgRFGAOhoO1AXZ9KIAOhri6HLaSl90ER9OK+0N58HhGq6kW9r/qMuqgTFF6Og8Eg19NxsPFAWvxlgyua7Yh5bL9IcXEq8x07WLX7yOvpNLWdsFHvpSL80vkiuNEdG6UvsPG7267WLakcX18HcJAW33MWrWReeL+jluGxlvmQJGldYkmqXdLkIslSVb2XiM4Ny5KMU1zQCWV8i5CL/abudTXY0UUj1F2XStgCHbdLUgkWtZvH7yJkEP4IAvyu5e5fWgku+jVkgv3dl/pvvK9YxuN1HHtfMCGA9h1jvfcrOiXXjmFskBZ/GYLtr3LdlOuFVLpOLeaFZdtxCGOnxcGAENYGbd7npd3PivdMBzAuCXNOCWN0QvvuvVvpvPB5GxWcpzEfkiStOwSyO1PpWlybyjIUY60jcz2eSnAjjNULKcGthj0u6E/meiPXMalcwD9OJRAyVoPBF6kEjI+6f4Pvc73SbU8LN7XX4EFw4945wiZ/wiMGEnAsd+U6L5XASicnqvfeRdzLFwNe3d9vYZwuWBt6CLALzfYnw6f+Redx1P191ca09JxF8Vuz/CkPju+7ZgyErPa9cN74cgDzQiDv62qOm5eI38F+qcvCc5jGfEiSpDlRO2qTGqRh6GHZti+YzAL3axEuwf1/LC223sp1YhhbrdWE4kEqHTbwJQ+C3yxtzvmQJElbgLfjwAgsF36eSifxpFyvp7I0OGsbcv2a6/lUuo50yuIf4+XvmsWx1XopDiyD4/s21xmpdOfoTr666BXTtbnnQ5IkbSGujANzZNy3Mlfr5lzbxcE5MYv5kCRJkiRJkiRJkiRJkiRJkiRJkiRJkiRJkiRJkiRJkiRJkiRJkiRJkqT/o38Ayp3tfsm/YRsAAAAASUVORK5CYII=>

[image73]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACsAAAAXCAYAAACS5bYWAAAA+ElEQVR4Xu2VsQsBURzHfzY2UUoZrAYZmMxKSrExGuVfYbMpxaBs/gOL/8BkVXaLVXx/91ydX3fO3b0OeZ/6LL/3XvcZ3vWIDIbfpwj7chgzKTiAaTG3KMER3MIrXD4vx0IG9uACnuER5p0bbDi2C+vwRNFiK3Aih2/AsR1Yg2t6EWvDi7wpSmwVTuUwIPx9EysxsW4EiU3ALKkzTptw7jJnk9ZJf7TH5uAYzoQbeHCZsy3rpD/aY734ymvgReyxK1J3Mgy6YvlxKsgFpkFqkZ/a28ML3MOyY987hI3lf2BH6rt2A/dw19CxTythYz8CX6W2HBoM/8gddtlGL8mYftMAAAAASUVORK5CYII=>

[image74]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADMAAAAYCAYAAABXysXfAAACtElEQVR4Xu2WSwhNURSGl1Dk/YiUdyKlDDxKeWRAGZAwk5FijJDHQErKQJFXUaKkpBh4TAxuKc8yIuUxUCJKRuSR+D9rL2fffS+Ta3Bvnb++Omfvc/bZa69/7X3MatWq1YmWirfiZ8Z78U38EPfFWtE3XugFnRHfxcKsjQA2mQe1Q/TJ+rpWQ8Rt8VKMLfrGiVd/6etKzRQfxGXRr+ibJz6LJ2J00deVWmVeK5vLDmmfed+2oh0NE8vFdGtvwVFihZhQtA9IoIFisRhadXemo9ZaL/3FRvOMbU/3eR81dE+sF4fFI/HMKivOFVfEbnOLzkjtg0XDvEYRC8hiHUj3HSkGZ/e6k66fmn/gpPnq5iIDBPJcTE5tI8RD83cZj1U/LqaJNeY2xa4oLB0umGS+CP8lmHb1woR3me9iy1JbaI74ZG6/ENlg9ckwwlZbzDN40TyDWBKRyTw4RPZyi68WD8zHZGNqiLvihBhZPdaqqJetRXtMOuwQamdJrr+aj5VrinhtzYHzfrmZ7Df/Xi6CbphnGrHAF8SxdN1W7SaHGKz0cliynAwLQXbJci6CI8gYO44AJhUTImOsOFbNxSJGphGuwT3tdtzf+tf5wkAEszNri2BuiUGpLT7COFjgkJiY+niXzIxP93Fm5WMuMK/BXASGzfJMU3/Ms8z+H80SH601Wq4vWXMwe8zrhzQ3rEo/vzrU1jlzW52yatulDvhVmpruF5lnKrJNTR0xn2iu2eKN+bfXmdfUDbEkfyjEoKxQ+T+WRx2TJKgN4rT5mcAqkwUW4Ko4KPaKF+KamM/LSVjopnhsHixjsbG8E+fFdfNzqBQWZ3csrdeRsN5K84wQSIh/tjFWZQcNL+5z0cfz8bNK5rBcZDAXtXTWmuulZxVnFtnpaWE57PvF/O+hPONq1arVoX4BhfiWYKhSqrMAAAAASUVORK5CYII=>

[image75]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABkAAAAYCAYAAAAPtVbGAAABhUlEQVR4Xu3UvytFYRzH8a+kiPxIiSI3LH6UQRZRkoFFis1gtTIhSUlZDCaLhDL4A6RMoigWoxikxKgkA4X31/ec67mPk3vv6Q4Gn3p1T9/n3POc5zw/RP6Tw5SgF6NoRn5QL0ZtcB07LTjFM3YxhW0coBX76E/enWUKMIdXTKMotVl68IQ7iTkS7WANbxjx2sIUYi+g11lnAh+YQZ7X5mYLs34xkzThHteo89r8rEvM+VgQG8WSV49KmdinzSq6TA/xLjHfMJPU4BYPaPDa0qVcbGlf4ULsZdUJBsWZ27ATpde/RRdHl1erwBmGnFonbtAWFvSmc0nfSSU2UOXV28VGoidCmG48Br/JrIjNyYBbdKLD1l0ftX/GxD6Rzq1G710UOx3C2lfqxd7mCNVug9iun8ekRO8fXdL6Pz3ftMMdsZfW8+1HEmIT9oJNjGMVx+iT6A7CT60Pzzj6oASGA43yffJGpUNsA7vzkfPoCHQkOqKcR/fHMi7FlqpOdGnKHf/50/kELv5AKwc69aAAAAAASUVORK5CYII=>

[image76]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABkAAAAYCAYAAAAPtVbGAAABpElEQVR4Xu2UPSiFURjHH6GIfMtCSUpKGZBsJilloRBlMCiDUiQy3EVSlBjMLBhksBkkk5gMSkpRFpMMRh//v+ece0/nvIv3vYPh/upX9z7Ped9zznOe84rkyBKlsBf2wXITq4a1dkASCuEKfISzcAbewA14CVszQ+NRAHfhISxx4tzBNbwQ3WEieuAzbPMTYBlu+8E4rMIXWO8nwAIc9INx2IPfcAnme7lmWOXFYjEqOgn9hOdwCpa5g5LCzloTncBORm8luoSJYKnYquvwXXSiaSfPbjuCD/BUMveoBZ7Be7gPK0z8F76UNc9zg4Z++AUXvXiNaKu/io6xdMFJ53+aJrgjek98OuAHHI+ID8EDo32WHchcABPcdpGfABOiZWnw4mySdtFdcDe8W6wEO5O7DOD94Gq7vThLyEPnil34sjlYKXoeVzBl/jMelJ2fiWM4D+/Mb7btFnyCIxI+xJXyclq4Wz47AMeceJpi0RUTtnAnHBb9AkeVj7Dm7u2vE53kxOSygj0Pl5ToRJHn8Rd4P1jGN9GyNjo5HvymhKXN8Q/5AdJ6Q5qesvrsAAAAAElFTkSuQmCC>

[image77]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADoAAAAXCAYAAABaiVzAAAAA7ElEQVR4Xu2WPQrCQBBGB0RQtLBS7HIERcTWSlCw9QI2XkAFG28g2HoC72HpObS001a/cQ1kNyRsyGZTOA9ek/19zRIiQRD+iT6cmB89U4ELGBjfczOCG3iFb7jVh71Qh3N4hDf4hANthgM4lA+ZkTqgrNApHMM9FRQawhuXFRqFz5dQF0ioZ1JDq7ADuxa2ST3hJllDWxTfO8nmb40NqaE9eLKUn/Dgu0onS2gNrim+d5JLtcyK1FAXZAktEgl1RRi6Mwc8w6EvODQH8rKCd1K/f6EPeCH1cPmgAc+kzo3eg+91iMwTBEEQvPABxW5Ek9AmslwAAAAASUVORK5CYII=>

[image78]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADQAAAAYCAYAAAC1Ft6mAAAC1UlEQVR4Xu2WS6hOURTHl1Dk/YiUdyKlDDxKoQwoicTEIyPFhAFCHgklZaDIq5hQUlIMiIHBLSWPYuJRHolIlIwoJP6/u/bq29++5zL47uC79f3rV+fsdc4+e732PmYttdRSV2qB+Cj+ZHwWP8VvcV+sFD3jhe6ic+KXmJuN4cQGc8d2iB6Zrak1QNwRr8XIwjZKvO3E1rSaKr6IK6JXYZslvounYnhha1otM++djaVB2m9u21aMo0FikZhs1eU4TCwWY4rxPgnUV8wXA2vmxnXcOvZPb7HePHPb031uo6fuibXiqHgkXlitLGeKq2K3eblOSeP9RZt5zyKCSMAOpfuGFR9gV7ubrp+bf+S0eZRzkQmceSnGp7Eh4qH5u8xH9E+KSWKFeclSuijKO6phnHkgusyhqv5h0bvMd7eFaSw0Q3wzL8UQWSELZBpRYlvMM3nJPJOUJyKjuYOILOblvkY8EO/MAxLiuWfJtjwbr1P0z9ZiPBYepRGqKk+uf5jPlWuCeG/1zvN+ucEcNP9ertXisbhl3meIQO+z/+y2VQtERLKs7SjPckEEgyyT7Vw4iKMxdxwPF622iZC5U+ZlG8JGhpeYH/Jz0jjPMF61AbXrX+cPjuLQzmwsHLot+qUxypRyZZ6h4ogYm2y8S4ZGp/s40/I5WSw9mYtgsRGRGTJ0wtwJskjmOtU08dU6nj9cX7Z6h/aY9xOTt5k7h/gtotfOm5fYGattyfQFv1UT0/0884xF1umxY+abRy4WHuW7yryXmJuxsjTbxcREqvx/y3sgFopj68RZ84gRbbJBEK6Jw2KveCWui9m8nEQ53RRPzB1mLjabT+KCuGF+TpXCienpml2WTWCz+bsNHe6U4VLzzERjIv7xRlgtS2hwcZ8LG8/HDy4ZpPwik7mif/Ke2iTeiAPJ3q0U/ZMrdsuqP5mmFiVPOX+w+sWTFfqusn9aaqmlxvQXdE+YZKj6auAAAAAASUVORK5CYII=>

[image79]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABsAAAAYCAYAAAALQIb7AAABoUlEQVR4Xu2UTSgFURTH/0IRhchHkRdKPspCNkpJFkpJsaCUrS2lkPQ2loqVjYQNFrKQlJUolLUSKyWyUpIFhf9x7jzXfZPHjJR6v/rV3HNm5s69554BkvwB2bSV9tIammriWbTUXIemlh7RB7pOR+gK3aV1dIe2x+4OSDqdpE90jGZ+TqOF3tMrhFyZTDRPn2mPk/PIoNtGuQ7MEH2l4zTFydks0wk3+BOq6DW9oGVOzmUBIesVha5q2on7kQPd8kDI8d6jLwj5xd+hhF7SG1rh5BKRD22Nc7oFXbVQDW2TM2jL5Jp4bDJRrr9CDlGzEyuga/SWdljxJjpojd/JoydIPJmsYpEWOvFGaKusGtNMvMvk4piB1sz+MhtpBfmL+PVfH22APiurq4feLy0kq46jHLrv+7TYyclfZIoOI77/ZCxx2R2p1zH0ZMvY7/4YEXpIH+kSdL/n6AFtg/+D8uWj1niAntJO2m/FfZEXRmi3sRIff3o/pCZSG48i6GSbJverePWyiUIn9K1XEORkztI7ugHdDQ85IHLg/LY9yT/jDVqAQh8U3mt7AAAAAElFTkSuQmCC>

[image80]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABcAAAAYCAYAAAARfGZ1AAABh0lEQVR4Xu2UPyiFURjGH6GI/M3GIotSBiRluJNYLBTFZlAGE4lsEooSg8XilrKYbAbdDAYWBiUSyi6D0Z/nue/3ueeee1Pf5473V7/u7T2nc97vPe85QJGIVNMEHaC1QayRNoUT4lBOl+kjnaUz9Ipu0nPanpkajTK6R49olRNXxpc0BfuiWPTRF9rhD5AluuMHo7BKX2mzP0Dm6bAfjMIB/aaLtNQba6MNXiwS47DF5Sc9o1O0xp0UF3XKGmzhcBN5g/ylioVKopbboO+wDaad8TqapPf0GtZF8oIO0ZJwotBiqmlWMGCQftEFL14Pa0/3kHvoE7xua6W7sD736aIfdMKLd8Iydy9VP30Lfn/R7ie0wg0GTMIWafHi2iyFzKXSV6/QUyeWRv2t7HrdIKxUOswRLy72Yc/BKGyjQ7qF7Jud3uWYztHb4L/ab5s+0zHknoXqrffGL1UOlbAMhVqxG5ZNAvnLJHQOD/jHI/YXyliZ6wsKhvp7nd7BWk4HWJDbW6Qw/ADlEEGmKe5eVAAAAABJRU5ErkJggg==>

[image81]: <data:image/png;base64,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>

[image82]: <data:image/png;base64,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>

[image83]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAIQAAAAXCAYAAADKtudKAAAFe0lEQVR4Xu2ZaahuUxjH/0LIfMnMReYhZLjJdJIx8cHVRaYbiQyZMmRIQjJPH2TImDHxwZDwQVFECcUHQ10y5MMlikKG53efvXrXXnutffd+73nPcWv/699597zW8/zXM6wjDRgwJlY0zjc+YLzSuFZ1fm3jbcYTjMdF3LS6PmD2gM+ONO6enN/JeE9Ffo+Fs4z7yz9ytfEd43rGzYyfGL+p+K3xM+OWS54aMFtg0b5k/Np4dHR+D+OFcj/uJ/cZfu2FNYxvGR+qjrc2fi9X397Gw6rzgI/HAxgwewh+i/1xufEr44bGlYzPa+TXGkgBF8hTwnXG7YwrRNd3Nc6tfu8oF8Qho8tLsI3xXNWfm0kcYXzcuItx44QYYOXRrUXEdohT40yDFbxA5Ugbj/Ma+RxT5ASxvnEfuY9WNb4iTx017CZ/cMq4rjw9/Gm8RHnnorLX5R8MwNh3GreKzs00GNe/BS6WC7kNBxu/MJ4qN/ClxheMq8U3TRB8B+fdbfzO+Jtxz9odDhbmx8Yz5U4lUn8ud3SMnCBi8G6iBWmkBhTyt/GY6hhRfKC8EfnoU8Y5yfl9jc/KBzhbIPS9KF81gQ8aFxkvU17cATvLnXB8dUxEwVjk4NzqmwQQBM6dMl6rvCAI88yTUM/vgBuNr6ku3jZB0AzgR/zWwO3yVXRGdbym8W3jr/LoEUC1epNxdbkgNoiusTofi467gsjCe5YWzrlOGC2Byd8nD4kx5hmfUPsqD0b+VKPnEc9JxtOr623YXO3jx14U4H2APXOCoH77QX49xrFq3l8SBGK4Vb7Ymdu29cs+GQwRDE4O/ln+spAWCFO3GLeQr5hT5AUlCMVJIxe1AIOfI09NiJEIxfOlnB3CeAk4HOfHzkNojCvUPiWQ5uiOuHcV+XM4sC2ixDjZeJfyothIHrXScL40lARB3faPmoLA6dgREQfkBMEYiT5EBuzJuEg9ReCQJ+XtSOhfSQNMKs7JGJCWE7AC3lRzkG041PiwRiuHd1whf++JqkcDHEr0QdldgTAQWEgBbQhGftl4v/Fi49Py1jrMsQ0I5zzjvaqLYlwxgJIgguNTW6fnST0U2LzjXXk9hB/JArEfIdGlARzyjFwI5M7D1R6iU6yjfvXD2cpvXuEACrkf5SKgLkEkR8U3dcABxjfUTUTBmLEDcCzOTIvnElJRLIsYQEkQnO8iiGnFDvK2ktyLUCYB8lYpN2Nc2l5WKuG4b/7lvazwriksGJMWLBY1xiVyEEG6IIjiOfmm0LhiACVBYJOc4ycqCCZG2uADrORJgbrlBuNH8lZr+/rlGg5Sd2GEGijOp20gvDLXtCgurcY2UGAyH8J1rqboipIgSo4vne8NBn1RxXgCwRipkaYLpAZaW1Yx//tgO5yoRMhNC0tW/FXyVrAL2Edh7Gl1XQKdFB1VOte+gmBOtH7zjAvVrCn6oCQItpn/UnNMQRDZeqAP+CAfTj+OcfhA17DbFziNPB+DToHCkvqB4gcDk1rC7mnXqp8WEqNhvByoKzbR6H0cv6dmb98nZQQxhDTBuxdqfFGUBEHdtUhNv2DPxWruG/UGIY7duUc1KsAIze/Lw25jF2uacL58AywHUscj8paUMVAht+0jxAjbsTljAubGLt8fqm/KkBrZ7cMeoE9RiRjoUPZKzi+LKBDE7xq19gG883q5gIO/eDd1C3VTqS7rBXLol8ab5f/GxqC/VOeXN4QWuCQInPuq3Plzo/M8hwg/NJ4mNy7Cie8pgf8lpGIIwIGkxAXphQxCp/eTPDoHkkrviO5DCMyBDgwf0b53bZE7g5U1JR/8geqv6P8TcCLpok/bDEJ3szzYgLkheMbK375zHTBgwIABAwYMGDBgMvgPvwAerQxg46wAAAAASUVORK5CYII=>

[image84]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAXCAYAAAARIY8tAAABo0lEQVR4Xu2UPShGURjH/0Ip3x8p+ZZEERKlJIPIwMJgMRkog2RQsphEpIwykIhSJosMionNYMEi2STKosT//z7nct+XlysGg1/96p5z7r3PeZ7zdIA/RCztpot0nKa4+VQ6S3tpj89ctx6YAdoECzRBD2kmzaMn9NJ5RU9pUeirgCTRfbrkxiX0mnbQetrm5kWn8x1KeRhWgklaRmN861W00D1XwAK0vi2HKKVDCP8uRDVshy00HVaORzqKD14mY3QXlplHPJ2nxb65VxboE+1yYwU5pjew3fppoOs0I2K+kW7ShIj5EHP0mfa7cTI9oPew7Dxq6BRNhAXI9q0pqxXfOAyllwXrEFFJb2Fl88qg+s/QAppD+2AHLOLoFqwSX6LDXoO1nHYslPY2LEtPtaNaVCijPVgWUdFLG7AfX9B2vGUUhDREqf9HlMPacBUW+NdRa6pMKsVgxNq30QGPOPXsoXoqQNTOCEodfXDq2UM/VoBAnfEZ+fSMLsNuRqFL7AjWqrVu7kfo0jqn07Brd4feuflfQy3WArvLmxF+Hv/8E50X2bFFXFlUYt4AAAAASUVORK5CYII=>

[image85]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABEAAAAYCAYAAAAcYhYyAAABWElEQVR4Xu3UPyiGURTH8SMp8i9RMqAMogix0muSgRKD3cAmKRYDo2SRQYpIionFrmwsdgwMTBJlUeJ7nPM+7htvz0OY/OpT7n3uc917ntMr8sfJQRcWMIXCzMfJMogV5KET6/534hTjCKM+rsIp2qMVnhKMYxVzqBe7gkZfukRflvFbWnCIFMrE/uMTJsU2yscepm251OHG10VZwjP6fawbneAWjT7XgX00Y1ZsfcZJFvGCER+na/Agdsp09ESVqJFPaqJVrkCuj5twJ3bFIp+bwZqv6cYuCvzZh2iBt3GF1mB+DMsYEKtPbfAsijbPjtjLF+iR95NptMDlYtdJ1B8NuMaWfLMzNT+yiR5d66JfTGsRG73fhAvvqo2lm2wGc1mj3/rRhd9dX9ZNtBFjU40zbKDU5/QrHIv1SpvPxaYX55jHMA5w7/NfirZ0CkNiPz6JeuE/v5hXVJs7mQeZvscAAAAASUVORK5CYII=>

[image86]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAFMAAAAXCAYAAAB3e3N6AAACz0lEQVR4Xu2YS6gOYRzGH6GIXE8OUS4LIgsnYkNZIJSUXBYkJbGTU84pZSUL4eSyIsTKtSxcUjaKIlYWNkpRLllgQ4lcnsd/XjPznplvvvnmO33DN0/9Ot+877wz7zzzf///dw5QqVKlSpVapS6ywm/MoVFkNzlN9pNJ8e5SaDjZRsZ47U3RItJDnpBfpDfeXbemkqdkBxlGVpHnZGH0pBZpHNlEzpNP5BUG6EXLzDVkNfmCxswcQs6Qa8Fvp4PkDiwaWimZuZYsIJcwgGY6zUfjZs4g79B/7DrYNXXtsugCSm7mMvIT/ccq4pU6NnvtWRpPNpB9sHkNjvQp1yml6NpTyExYnh8ROaeWSm+mM80fm9aepkFkKyx/L4WZdpjcJRODc/aQ97DrXiSnyBtyA/Wlk9KbqTFJpuU1czmsQCyJtA0lVxA3y831OiyKr5KjiOfrNGWaqRt2wk7IYgLiy8apiJndSDYtj5naAdwiz0iH16fx38ni4LjIXDPNnAfb29XDcTLtz6i4ikwwzbS09iQpGF6Qe2RkvOtv5OulSUXmmmlmM1RkgooYRY4/1pmpqp4lRaOiUvlyrNfnzHSFzM1V18+rUpipKpr21TCZvCQnvPad5AOZHRwrvSjNaEn7Ur5TQXkNq9JRab+qXDo3OP5nzNR2xNcsWAXVg073+iRV4QPkERkdtLnCIYNcYdiOsAonFQt9zn4kWyJtKjCPyTHYfSQ3V7fs80hmJr2wpmgXeQt7SIce6D4siiTdWJ+K2kumRYNMvE0uwz4lz5EHiE9aY7/CcmNaZMwhD8lNcha29PfCXo7UR74hnKvmrn1uLek59DyfEY77ATNVz98S6btbRqVJy1hRsz74m7Rr0BI/CSs4taSUonOcif+VtCyPIHmZ55HG6zpJy7xttJIcQpi3GpEiVUZqc962UhRtRFhcGpXypP57U+SFVKrUZvoNd/Gkf5ISKgoAAAAASUVORK5CYII=>

[image87]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAoAAAAWCAYAAAD5Jg1dAAAAVElEQVR4XmNgGJpAAYgj0AVhQBOIs4B4HxD/BeKFqNIIAFIYAMRWQPyEAY9CGJAE4ocMowpxAJjCpUDMiCYHBi4MkIAGxcp/KP4CxJeAWBdJ3QgDAI24GJNXncOFAAAAAElFTkSuQmCC>

[image88]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABMAAAAXCAYAAADpwXTaAAABfElEQVR4Xu2TPyjFURTHv0IRZWAxKMmfTAZFCpmUwYJBWcRAmTCIGGRiUCwkKVkNSgYxvJlFisVEymZQlEF8v8497933/C2D5X3r0+/ec88995x77g/I6i8qJ3Nkg8yT6vTlNNWRJZhvL8mNF5vIEWkjDeSAvJJJkhP55ZNZckoaSRXZJ6PuUEj2yBBSJ5SSE/II2+TSpitSGeZdsEO33UHlXZMHWFauGZjjRJhXwAKtJj2AElimzW5Q6ivkEBbYNQULpq/UH+YDpADmWxzWvlUe2SUvpCPYlJGCLZAdMkbOyCIsoS+ltHVf6pY76l4ULIFURjXkDtaoT6V7OIadXhTZPdhIZFPQBLkgZZH9XcpinSzDuhxrExasO7J5sMyuJwNNI/VE6klnGKurvwqmh6m6x8PYpZJ6wriFPMO66fpQpjYPkidyS24i7kmrnGBl67WrXHVb+tAAf7QqIRM56pdxqexLskWGyTlZww9P4ztpYzvpI7VIv5as/lNvKc1QQMAKIC0AAAAASUVORK5CYII=>

[image89]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACcAAAAXCAYAAACI2VaYAAAB7klEQVR4Xu2WvyuFURjHn5sUUcggRV0SGZTCYpBBykCKJGw2mSkLJQODYlE2g/IPmIg7ik1hMIj8mJgwKPH9Ou9xn/c4ly7Xr3q/9em+73Oe55zv+77nxxWJ9EbZoASUeihUeb+iOFgAyx6Gk2n/THHQ7waVasCcmKccALnh5hcxxjbmMJc1qZQF+sSM61UtGAFb4BGshJtf1QMOQT3IB9NgAxSoHF4zxjbmMJc1rLWi+U4xn/YC3IEG1R4SzXWDZnAufnPl4BgMqlgR2AOjKjYexNhmxZojMYuAorkO0Aom5QNzVlwtp+I3xwHcTmJgFSTEvCVr1q1vAregy4lTfBi3X6/eM7co/k6YewUqxXyB6yCmxRrWzjhxKiPmGPN1ouPWhFufKk592Rw/WUL8nWhznORPQUzrW83lgU3xd6LNcZL/uDnqz35WipPZ1wlzuf2UiVkUXBxuvTU34cSpjJjjNsANuk3FcsB6AK/t3LT3Vqx5CH5dpW2Oe1fMaSsGu2BKxarEvDV93A2BM1AR3LMfnhY7Ej5JrGjuXsxe6BWfiIPwzXBCE26a+6BO5TWCEzAGesVsuLNi/gJZ8XoJbIs5dWjsQMwxZsUFtgZuJDkeuQTzKi9tseN2MQPzSPOJb6tazAO0SNh8pEiRPqNnbK2LFmzptOsAAAAASUVORK5CYII=>

[image90]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADUAAAAXCAYAAACrggdNAAAB10lEQVR4Xu2WTShEURTHj1BEITaiSDYWkhQRNQuUhZKVspGNskMSZWFnpchCNpIslKRkIZI1ZaHskbKzEHY+/v857857c703H4t5k3q/+jUz95yZ7rn3vnNHJCIiIlfUwmW4DVdgc3JYyuEOHIf1ovleK93U0GiEY/agoROewz7YBk/hD5yDBU4OJ/7ojPu54eTlmhY4DS/hF9xNDiul8BhOwkJnrBpeww/Y4Yzx9VZ0J72ewHvY4OTlGhY1AnvgswQUZXbgTXSXDEuiOzDrfB6GU244TjHcggPWeBiYefsWxYmtwzPRRMOCaFF8JVyhOjcch0XOi3tEwyRlUX4UwUPRMxtLDiVoh0ewwg6ERNZFdYk+T3xmuJM2HDuQFJ0nBLIqiit/AfdgmRUzdMMH0SOZDh5NNh77CgiyRL+WloyLMg/+mmhX9IOT3IQ3sMqK+cH7a1X+ds0gh/RracmoKFPQoritnTsxmMhQakRb+JXohZwv0hbF1edFO+O8N7C7jXo+E7Z9tn82EjaUfJGyKBYxAT9FL7Mnj6+wN5Gp9MNvCfixEDFF7YvPlWKC9t8e+gKb3NQ4PPOM5asoLioXn9eNmec7vIOtnrys4LMXE12MiIiIiP/JLyUMZdCmPTtOAAAAAElFTkSuQmCC>

[image91]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADgAAAAXCAYAAABefIz9AAACm0lEQVR4Xu2XTYhPURjGH6HItxH5KB8LspApw0KShY2FjxDCwsZHNmpMTSlZTDYWio2SNBY+syWlUCyUFYWyQj5W2GBBPp5n3nvq3Pd/zr877pSh+9Sv/9xz3nPf89x7z3vOAI0aNRpumkd2+EankWQbLDalsWQnOUtOkEXl7pIUu4dMdu1BE8kh2L2Okpnl7mpaTA6SO+QHuVDuHpAmsp6cIm/JF7KsFGGaRG6TPjKedJJnZEsUM5VsJ/3kE3mF9MTnksdkLxlD1pEXZEUcVEUyuImsJG+QN6gEa8gx5A32kkdkStS2izwnM4prGdxIusgVpA2OIufI9eLvoOPkFmw+g5aSKFnKYCyZSBmUKZnz45eTz2SDa5cUmzK4gLyH5Yq1GenclVTXoL6ED2gdrzjF6+l75QyuJT/RalDL5Bfsqxi06hoMRvz4XLuUMxiM5Az69kqqazAk9+P/xKBypIz8VYMqQkNlsBtpI96gis0+2Dai37bFp67BnJFcu5Qz6I2k2lVdj5CFMGP95BIZHYK96hoMlc+PDwY1Ga+cwVXkO/IGVU3DfPW2JY15R5YW1y2qa1Ab+z1yA7YxB6kifit+vXIGZ5OX5LRr3w+r1KrYI2Cb/rSiT/eXQfUlFQxehA3OSQa/wvY3r93kNZlfXOs+OtU8hJ1yvGRQh4s5rj01Tp/eNXIZ5c1fCgeDM4m+AedKomOaXr/QxvyELClixsFOHR+jGKEndrKIkTQJJbkLOx1pkk9hR7ag6eQ+LEe4j3JrDgeiOBm7Sa7CCth58gCtD0PSJ6u33bbIDJX09LXwt5LVaLPoK0gHey0F3Uu/uvaS+cOwPHp4Ogr+N9IaVPGaBVtiPUi/4X9SHbD/NuIlo09/QhzUqNEw1m/lj7FLP1oZ4AAAAABJRU5ErkJggg==>

[image92]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADYAAAAXCAYAAABAtbxOAAACjklEQVR4Xu2WS6hOURiGX6HIQRxxFLkkl1KIkkJn5BoDDJSBU8othRQyMzCgDBgYmLglBqZSGLjMGDNwSeQSdYgyUS7v+3/rs/deZ6//l1ZJ7bee+vfa317re9flWz/QqFGj/0kLyIq4MWgQWUxOkzNkLRlciTDNIifIWbIR9TGjyF5YzFEyE9Z/VinZg+Qh+UkOVV+3pEEVc5dMI93kMiyxoaWYPvKELCNTQsxFMjzESPPIHdJLxpAd5Bs5gMzmZGwdWUO+ot7YQvKeLC21TScvyarwrBh9v+d3hMW8JZtLbVrx72R9eJY5TWo/meNBOeWJ1Rk7BjMxsdQ2ktwn52AzrYS14pokVxdsda6TYaHtJCxuW3j2fr7AVjO7UsaUkBKLjXnSmm3N+gWkjZW/1dYdh+LszSWfYHGKz66Usbrk6trbGUuthoqIzuErMj96l00pY0payXcypnMkYxtKMTpj72D9qn/XCHIVZug5WYn66tla3gmwAToxHvWdpIypXw3eydhocht25pSPzt1h8gMDjZU1G1ZgLsEMV6RlVOn9E06Rqa2vqkoZiw20ax8L21pvYGV/F7kXxcTSBOgbrfbO6F0WpYwNIdcwMDk3poqmylYnFYlHKKqiVnJ/wO8/SWPKmM5pdqWMSWqL7xlPWmVemgS7wPehuGjVpwqH32M+Rrw1vfB4X1nlgx6JX1AzyGtUL1r9u/hAloRn//4KbJW1Itr6N1GU8cnkKTkPO5NSN3kAK/n6S5dN2tc6vJox5yNsi6nQuFTtXpDtZCt5THajWB0lfwNmZgvM0C3SE967VpNn5DhsorRNP4f2fybNru4pod+xtEqLyCa0/2Or89YLi1uO6nlr1KhRo7/XL1QxnIM41ooAAAAAAElFTkSuQmCC>

[image93]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADYAAAAXCAYAAABAtbxOAAACsklEQVR4Xu2WS6hPURjFl1CE63F1UW4ieZRCbkmhO/KMgUcpA0rhSiGFGBkYUCYGBiZeiWIqhcHFjDEDj0QeUYgyUR5r+fZ29tl3nwediTqrfvU/e39n72/tx3f+QKtWrf4nzSfL4kanQWQhOUVOk9VkcC7CNJOcIGfIeqRjOsgeWMxRMgM2fqNSsgfIA/KTHMx3/5YmVcwdMpV0kkuwxIYGMVvJY7KETHExF8hwFyPNJf2kl4wlO8g3sh81zU2ATTwi7ogkY2vIKvIVaWMLyDuyOGibRl6QFe5ZMXp/958Ii3lDNgVt2vHvZK17ljkt6gcy2weVaRI5T0bGHQXyiaWMHYOZ0Jheo8g9cha20kpYO65F8tLc/eQ6GebaTsLitrlnP84X2G5WqiljSkiJxcZ80lptrbrmKjIWvqujOx7Z3ZtDPsHiauXalLFUcqn2MmNFu6Eionv4ksyL+grVlDGNo+SrjOkeydi6IEZ37C1sXI3vpXt/BWboGVmOdPXEGNjgIVqhq2R6oi9ltsiYipAmrzI2mtyG3TkdN927Q+QHBhoLNQtWYC4iKnR6OAKrgCHa4uewieI+f3FDFRmLDZS1j4PN+xpW9vvI3SgmlhZA72i3d0Z9SWmgJo7iEHINA5PzxlTRVNlSUpF4iKwqaif3Ofz3T9KcMqZ8K9WUMUlt8XfGJ60yL02GfcD3IvvQakwVDv8d83PER1N5ypgfq1T/auxw3AG7p6+Q/9Dq38V7ssg9+/cvw3ZZO6JjfxNZDt3kCTkHu5NSJ7kPK/n6S1epusZ0rnV5tWKej7Aj1hXEqdrpzm4nW8gjsgvZ7mieGzAzm2GGbpGJrt9rJXlKjsMWSsf0s2uvpbrG/kZaXX2nhH7H0i71kA0o/2Or+9YLi1uK/H2rlAxthB2LVq1atcrpF71Xpl32D15lAAAAAElFTkSuQmCC>

[image94]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGwAAAAUCAYAAABoF9hhAAADIElEQVR4Xu2YW6hMURjHP6HINUQe5E6e3EIKD3KJkDqKKB6ko3gQoaQ8yINrLg9KLiF5IB5OUpImXsSLlEt4kksplKIk8f+dtVazZ8/eM/vMnEzj7H/9mva31t57Zv3X9+1vj1muXLly5cqVC00Vi+JBr95irTgjTooFonvJjKImikPm5rZY+rxcNWiW2CUeiz9id+lwuwaIG2KTGCv2id/ith8L6in2mrvWdDFGtInNkTlpWiKmxIO5yoVhy8VS8d2SDdsqLon+/rib2G/lBmPMazHKH2MCcy6GCRVE1vI9/rUmiyPxYDOIjEgzjAVn4bdHYjPED3FX9BEjzJnFwgeRfWQcm6KSBotH1hjD+N2n4sFmUCXDVopnYmEkFuYXRF+xxpyp60QvMdzHq4ln4zFz5643dx4GksVRsSE2mNs0E6x8vFb9l4YlCWNY5KP+mMzimFJ5WWwRT8RBc8+2JGHsTnHH3Ll80qgQYwyF5+I9c+VrvLgqLpjL7HrVJQyj1N0Xr8RIHwtls2DFzGJxP4od/jhNlELOTSqJPBffiXGRWCihx63+TGuIYezCYebKSTWGWnKbndUwFoiu8o2YFIkHw1ojMYwrmCunQyLxuNIM4xzOvWXFjAvifmwGOtEs4ntjdHw9FotzCXGI37PTRDtMKcnCCSt2cVFlNaxFPLDya5y18kUPhnFdrp+mNMMogd8sucsMG4RONIvYqIetfD1uipcJcch67YYoi2GYxbvXIH+MIaHJoBmIL3qthk0zdw8y+LO4Lnr4sSAM+yXmxOIdVUNKYmeommEzxRUrfVFmQclYys1s8dOcgUG1lkQWkO/DvR6ay+h+fgxhHia+MPcoqEdNb9ie+IA5Y3hmfRJvI3wRB/wc2vM2c6UxZEPWpoMsIVswm3PZBKP92ArxVcz1x4gGhEYkyz8o1dR0hvGjP5jb4QGMYFdT91F4XiQRzSiMfS7Oi43iqTht6W19EOPMe2/uL7BtVuz++Jxv7hXhmrnvwuZZHZlTj5rOsM4Wiz9PrLKOv+AO9CQpdHlpXW6t6vKGNZto35fFg7lytesvN6u2aXwAygEAAAAASUVORK5CYII=>

[image95]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAH0AAAAUCAYAAACgezK3AAAD2UlEQVR4Xu2ZWahNURzGP6HI7MpQZEimbpmTQvfFHA+4pQilTClThq4nDx6QMpQHJVMiPCikUC4eFM88GBIZQojyQobvu/+97H3W2fts53CPe+/ZX/3qnrXWXnvv9a31X/+1L5ApU6ZMmTJlypSv0WSaXxioFZlADpBDZDZpndPCNJTsJofJfMS36UzWwdrsIENg/Wcqk2TkFnKP/CRbc6sbJEPU5iYZSKrIKZhpbSNtlpGHZDLpH7Q5QdoHbaSRpJ7UkG5kJflKNiHd+K5kOWnnV2QqTjJ9DplFviDe9LHkDZkUKRtEnpEZwW+10fVrf7ewNq/IwkiZIsV3Mjf4LeM14d6T4a5RglR/jnT0KxpZvWATvINf0dzlTIszfSfM4D6Rsk7kNjkKW6EyU5FCE8hJ5tSTywhX515YO61YyfXzGRYFCmkxuYHym673Po7y37fRlWS6zJJpvunOUK1SrVYNSpLp0Wu1HfRAuNdXk4+wdoUGdTx5Tu6QwbD+/DCvbaeW1MHeJy6fKEUVZ3qccXHlhUxPWsVK6LTvy8xRXl1UqlNEeRegvxVu3TWKNEtgE7AGlk/sIddI76DN36jiTNcLy9g007Vvy/R5kTba01/D+lX/Ttobz8DMfkKmI31VuvsJf/CnwqKFEkgnRZSz5CJyE8lS1CRN1wsq2dDDpdET8QOcZLr6lTFppnch12GrUM+j1beN/EC+6VENgyV7J1E4UUoy3W0/92HbRlR6l2/ITUDTpBOCP2aKUkog3bYS5b9NBIU5hbs/YT8Z0HBVrpJM980tVN4dFq5fwo5uq8ktr40vTQ5doyixyquLKsl0Nyn9cknvon43euVJ0qTbjvwx0/M9RbitRHEJabNUkultyHnkG+dMUOatDDxOWnlagS57VwTYEODO95IzRyE0Sb7pmmBjEN7DJZRRuX4XeeXFSu/d5ML7v1CS6ZLK/HO0G2wd1aS+sI836xF+ZFGfSuLcOd3dww/3GlCZ4/qKk2+6rj8Im5SnyQvYM0Slo6b2+mqvvFi1eNPr/ArYXqZBjX5kUdL0lkwMfrvrZYCM0EpW+LuKcLD6kUfkGCwHkKrIXZg5+gycJBdx3IrWPq2cQdJ1H2DneCfX7z6kf+lLU4szXfuoEimtNIcGUGFbSZ+TsnLtayvIUvKArEE4oBqQKzCjFU5ldtyRaSZ5THbBJpFC/6egPE3jYBNN97kAO5o5jYCd4S+RI7AotBm520ipanGmFyOtHp3Dhf72pQGWMQtQ+J8o2t9rYO2moDhj1FaTMekaZd9K7pLqS1FFm16pktm1sC0mU6by6xcyyuBvw/hnlAAAAABJRU5ErkJggg==>

[image96]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADkAAAAUCAYAAAA3KpVtAAAAoElEQVR4Xu3TMQrCMBiG4Yg6uUmpCI7FCzh5gNILFNwVN0c9gkdw9AC6ewcn10L3HkPwDUaaBtwNfi88BH7+IZDWGKWUUkqpf6mHFRo8ccGss9E2wjYcxlCOG+ZIsUGFpb/kmmAXDn+9AY7mfXm/DHes0Xcze+5RfJZiKcEhHLrGuKLGGQ+cMPSXYsheeBoOvez/aj/jEgvTvqpSSqlvvQATtA4+ktKtVwAAAABJRU5ErkJggg==>

[image97]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAD8AAAAUCAYAAAA6NOUqAAAAn0lEQVR4Xu3UsQnCQBSH8RO1shMxCJbiAlYOIFlASK/YpYwjOIKlA2jvDla2QvqMIeQ7PMl5Gzz5f/Dj4PGag0ucU0oppZRS/1kPBRq8ccX8Z6NrhEM6tNwGdywxxR4vrOOlUIYyHVptgJP7XCpugQd26IeZPyvk3yXrTXBMh6ExbqhxwRNnDOMly/mLzNJhlP8f+M9hi5XrXoFSSikrtSq5Dj7jDASMAAAAAElFTkSuQmCC>

[image98]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADAAAAAUCAYAAADLP76nAAAAnklEQVR4Xu3TMQrCMBiG4Yg6dRNRBEfxAk4eQHoBwV1xc6xH8AiOHkD33sGpa6F7j1HwDaY0zeoQh++Fh8DPPwTSGqOUUkqp+A1wQI0GTyx7G10JzuEwdjvkWGOGE0ps/SXXHJdwGLMRbuZ7Mb8V3jhi6Gb2zJC2S//QFNdw6JrghQoPFLhj7C/Fzl5mEQ697P9hP609NqZ7DaWU+r0PcSUOPvwnNKQAAAAASUVORK5CYII=>

[image99]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADUAAAAUCAYAAAAtFnXjAAAAn0lEQVR4Xu3TMQrCMBiG4Yg6dRNRBEfxAk4eQHoBwV1xc6xH8AiOHkD33sGpa6F7j1HwDaY0zewQ4XvhIfDzD4G0xiillFJK/aoBDqjR4Illb6MrwTkcxtgOOdaY4YQSW3/JNcclHMbWCDfzvazfCm8cMXQze2ZI26VYm+IaDl0TvFDhgQJ3jP2lGLMXXIRDL/u/2c9yj43pXk0ppf63D678Dj4/ZrYSAAAAAElFTkSuQmCC>

[image100]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAoAAAAWCAYAAAD5Jg1dAAAA4ElEQVR4Xt3Rv2oCQRDH8ZEgCPEPkrS2QiCgYi92ksbKIn2K2ImkSG1pYWNppYXgC9gLdvoIamMvopBGkPid21mDJz6A+cEHdueG29k7kftKEg300EIWkYsOksMEZaTxiQO+JNTcxRFV22vzHBu8+CZNB7/4sH0CU+zFnXZOFM94sP0rtuLGiVvtKnqpIdbIh54FecRIXMMKFfk74Wb0pvpWnbvuizpf0+ja51tc48AXivgxuvbRBm3UTxckgwX6SFntCTNxNy9YLcgblmjjHWPsrH6VmLhfWENJLuf9XzkBk7ElElgEQigAAAAASUVORK5CYII=>

[image101]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAFIAAAAXCAYAAACYuRhEAAAEJklEQVR4Xu2YWchvUxjGHxki85ATkX8ydoydkEJHyXBBMkRxIefinCtjiKJzTmQWkrmQxAVx48KQRLngQqRcoJAIIUJRhud33v1+/7XXf+/9/774lNpPPX3ttddew7Oe913v/5NGjBgxYsRisId5vfmwucHcr/26E2eat9WNxg7mpZqOdYC5WavHf4MDFetjHVcq9jgE1niZua6j/RjzXvN+81xz21aPBkebr5jHm4ebL5p/KSbvE2BifmI+UbXz/evmanNnc635u4bHWg6cbT6nWA/7elexjrPKThUQ6xfzmqJtC/Me8yZzL3N/8w3zA3Ofop+2MV8wLzY3b9p2Nd9WDLqqaSuxpfmoQuxaSE7tD/OM5hkx3zG/Mw/OTsuMFeab5kmaHh4R9qX5kbl301ZiR/M1xZ5KIVkza3/Z3K5pu0DRj70uALt/Zv6kOL3EdYrOVxRtCax9p/mFZoWkne/WNM/bKzZVj9+FXRr2gYPeU/OdzeFjAiIGUQHfPKVY22lNW4J37PN2zTqS9PC1+Z7CYABXMw5mWgDuwrovqZ1DGKw+HTBR5JyDFAdQC8l4u2nq7kPMHxThnifaB8LmeVUh04BxbzCv0nwhcRdh/Zjac7JW9nR60QYI6TvMYzUrJNhJ03GY+z7zTw2niU0gLzyrCNHVRTubYUImTifXQpbg0sEFn5tHVO/6QD8OtRRzKSL2IVMM7sJlCUTHGBNNnVwLmWDuU8zvFTqwrkFk0mWCsjMJnBBgwCEhudGeUQhIeDF5OnQxKMX8N0QE5ymMcbWm4/D3ckWqAkNCkm/ZzzfmQ4qoGwQn9Kr5pNpXPJt6pHkPhoQsQQogydfjzQNikvyZ85+KyNo/NNerbQyqFXJ6tg0JmcAQNyvSFeJ2ggEfNO9S3OYJQv0WhVMTixUSATLJ1/XZEFgLtyKO3rd6txRw8JRz1LVlVJDz2OukaFuMkOAo81dFCbR79W5BxGs1nZCr/2RFZ25erJ3EZYhDbcYzIjEGoQLLk8+La57oCb7lBsWJKxVCdF1A85AiEtbp6BMUbj/UfF/tPRG2rPPn5hnHHaY4UP4m0kSI3ioPmYSCGQHKEKKY7ruZuhyZJ1pPQB8W2Kq7elCKmGvhQJcqJuPcrdn136hwVBe6HJlr79pnqzZmsRcprEpdWJ4Qt9Nx2bECVT79CdvcMIUuBe/jmubSLO7JKUc2bX1g87dq9kDBUsRknPXmb2rvh/V+qv5UkSFLDZ1A1G/NU4u2CxXiPqBIe5uQzuJFza80O2neyIR09kPwzH8Uux8rBDlfsfkfm/Z5ONG8RLMiJvh1stHcqn5RIR1T7wdSAlEKlSB1vaW41bMfYU5oY4inFRcwPzKIXEIfDdIsy4atFfXnOYqcVObL/yM42IninzSw6yfmiBEjRowYsbz4G1x28baxQ583AAAAAElFTkSuQmCC>

[image102]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAFIAAAAXCAYAAACYuRhEAAAEK0lEQVR4Xu2YW6hmYxjH/3KIHJOcy84xGZFxSHGhkHJIKEJywUxTTEMZLkSRMqQcyhTKINSMuJIc0h5cuNJcOJTIIYcioxQlGf6//azn+95vfev9vm/vnXGz/vWvvd71rrXe5//8n+d9vy316NGjR49puMh83lxhHtbiIebuzbxdzYvNJxteZe7V3CvB2LXmU+YD5pGjt3cKbjUfNI/TeEwHK2IB5VofN88v7pU4wXxIMe9Kdc/RneY/Ff5inqgQ8wnzXvMY82bzd/Nj8ygNcai51XxUIeAl5lfmOcWcnYHnNB5L8hPzIHN/81VFLMREbH+bbzT3wC7mjebn5rmKWF9UGG/MRM+YrynUTj5tfm2uV7wM175jHhGPLOB6xcJ4freG/P2ZwskJXPmhhov7r7GP+boi4DKmTeaPCkeBWxSC7NdcE+f9ipgwF1ipMAxzE0ebP5jXFGMLH92oyFCJs8wXNFQ9XYtQCRz3nfmlQjici4PnFe9NXGruUJTNJBzYsAbK6XBFwJNA+dJ69myNX20+rOHz6drbBzOkM8w/FKbZW1HuzCGGBLHNK5I1+AZCIRpuStBDXtFoyZ5ubjNXFWMs+JuG/J3Zm9e4kGWWa6CfURnldxO0lnvMOzRdSJJxWmvsJPMljVbF5Yoyv6AYa8eQYncJmXF3AkHJAtmbBvreX4rgyUx7EYkUsnRzDaeab2pUzMWI2AXWskVhmGm4TrHWR5rrSUL+Zp5SjI+Ahvq2pvczgnvW/NU8sxnjGXrh++a+zRigR7IYFjULSjGXKyKgl+PGsuq6wPrfU2wsmUj6IGu/IicpeiS9FtNgnjHwoZcVjpwGGjYva/c9xn/WMPtzis1nMUICxHxXsektR8RM7ur2jRZ4PxvrF4pen+B5+iWmIanMu0vR86tCco7EYdh7EnDgR4rG3AYf4sjDgughlD1BzNIjS7BoEspGhgOWCk4bBDzt+IUBqKS51jig53IC+F7h1jUK51Z7ZAZc9oM2EPED89jmGhdTOgcMZoyD986yaycQkd0VJ7JJsDt2bUCzgGRUndMAETk75omBHoiZ2rt+ghMOmxTr6pzDZsDmUcsewbR3c17KGS17Ir8oyGzOwaGUxaznyFLELGdKbSlicnyhLGlBNVdjDNxWro3vPab4Pke8rea65hqQFDaakXNkAmVZbC17/GLBztvNbwv+ZG7WsJHTB/80z26uWSiBXNZcTwIibjBv03hPXIqY6ZxaCfJOWhAxlDERIxskyJMIewcxskaM85ZGTyYDZPZqQuaBvIv5UYBgn5o3mXcrDuw3aFyYLpxnrlV9Lu3kPnOP9o0K+JFAj60JSdLbsSRzn0Asyh7xGENATjUYqwqyTVl3/iBfBAiAPnuhIkH/J05WnPVqyZkFuJAfJPyT5ngt7109evTo0aPH0vAvFs/tuWvlINkAAAAASUVORK5CYII=>

[image103]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACIAAAAXCAYAAABu8J3cAAABYElEQVR4Xu2VvyuFURjHv0LRVQYiZbBaRJRVkYEsshkN12CiKGWSZFEMlPwHlMHAKEazRRkYlMVmUX58v/c5J+e93LdX70mp91Of4T7neW/fe885zwsUFPwtA3S8uuiop5N0j+7C+hoTHTkZpsv0mn7QleRyhRZ6RBdoBx2id/SMtgZ9uVCQKTpBX/BzkDH6Tvdpg6ttwIKXfVMsBlE7yAh9oye0ydVWYUEW3edopAWpo234CtFMz2H9ei4qaUFCdGjnYL06MwoZlSxB5ukDfaJrtJRctmvUSbsyqFOvX1VNliAebc0x7Ob0hgv99CCjO7Sn8lSS3wQR07DDegoLFo20IKN0m3YHNd9/D/uno1EriIbZBb4PO80e1W5oe1DPjQ+i+RCiAXZIb2mfq+mmrMOCLLlabnQTHmFf6n2mV7CDLbQll7AxP0s36SvdQuT3TRZ003RDZmDbogFXUPD/+ASP+kkGEdNN3AAAAABJRU5ErkJggg==>

[image104]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACIAAAAXCAYAAABu8J3cAAABgUlEQVR4Xu2VPyhFURzHv0IRRREpo8VCoqyKDGQRk9HAYKIoMkmyGAyUlAwmyvAGRjGaLUpiUBabRfnz/fmd0zvvvnNeBxfL/dRneL/zu+9+37vn/C6QkfG3dNHBZNFQTofpFt2E9lUWdPyQXjpPL+k7XShc/qSWHtIZ2kR76A09oXVOn5dmukNrkgsJJMgIHaLP8AcZoG90m1aY2io0+JRtCtFC96G/JoZuhIP00Vd6TKtMbREaZNZ8DpJmkDLagHyIanoK7ZfrSpJmEBfZtJPQXtkzErIkvxFkmt7TR7oMz/6rh97YtRO609s8a75wMUEs8miOoCen3RYl1RL0hLge0Fu651mTvzbJV4IIo9DNmoMGC5Lmo+mnG7TVqdn+O+i9gqQVRK4/Q/Gwk9kjtSva6NSL+G4QmQ8uMsB26TXtMDU5KSvQIHOmFiQ2iJyEB+iXWp/oBXScC/JIzqGbf4Ku0Re6joj3TWyQWGR+yAkZgz4WGXBRSIBx5N8NGRn/ygd0wlLgeqfaZwAAAABJRU5ErkJggg==>

[image105]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADYAAAAXCAYAAABAtbxOAAACk0lEQVR4Xu2WTahNURTH//IMfDyEiSL3TUShDPRKMpCp0pNShgYkUeQjpcTgvXwNFAkp6fUoMpPBC2UgBpQBA8lHYsSICfn4/9/ay9l3n3vuPoOnZ3D+9au799rn3LX2XmvtAzRq1GiiNJtsJpNSQwfNJHvIRXI4jFPpPf3kLDlPNpApbSvK2khOpJORWmRLOpnTfHKVzEgNiZaSF2Qv7JntYbw4WqOg9pNzZFHgJLlDZkXrYrXIa5gPsfR/O8k98hNle1Z1ApPtAblLpoa5HnKTjITf0jJym/SGsaT1CqzTjuskL5PfKDuuwHSSq8kHlO1Z1QlsPfmF8ssPks8wJySl3VMy9+8Kk57TSadSCZxGd8fl3ztU2ytVJzA53GlXFZjmZZfWkB/kCYoUbZHHZGUYuzSvWl2C7o5PaGB+GkotOau57+QMeUgGgt2ldadgTSbneM6O6eQ6eZ/wEeaE0iG1HRt7EugL9mEU3dNrTEEoQNc0ciPMi5dkRWSXNsE2Q+/KOZ6zV6rOicmBA+QVLIUk7fYXtAfmJ3ENVvTPgv0bLE0ldcpLKLpkzvGcvVJ1ApMmk12wk3wLu6OOo73GdpD7KN6ljngI1q7VUdUth2Cb4so5nrNXqm5gnaRL2LuiUn0Udr+l0obIueWwmktLwWtSY21OrH8amOpJbfkWihTyu83vMR9vDfZYq8gjMi81IO94zl6pOoG502ogfWFODeAT2tv4Pti6+CtDKTxIjkZzsRag3JhieWBVdsyB5Xna+bp1xSNjT5rktHZdJ6IUfEPWRXZJNXUBVoO7yTZYel6BpWos79L6b++gakaeivookE+qT7d/Jc9hKT1u0m61YJ85a9H9w3YhbJ3Q70aNGjX6v/QHjVitKz/91oMAAAAASUVORK5CYII=>

[image106]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAD8AAAAXCAYAAAC8oJeEAAADJ0lEQVR4Xu2XS6hNURjH/0KR9zshwsQIeZTZzSsGSh4lxigZkTe5eQwMDLxKUqKE0r0DeQwMTkxEYUBKxJUoQimS8vj/z7dW+zv77L3vPjcj7X/9Bmettb+1vvU99j5ApUqVKpmGktWkV3oiQ2PJPnKGbCUTGqfr6k/WwtYcJwtJbzc/kJwj68h4mE2PzuPl7R2GPRPVqq0madF5mKEiLSM3yHQyihwkP8gKt2YI6SDryRSyn/wiN8OcpP26yJ8cdGFR08hjsgvmnIJ0n4wO863YylQZ5/uR6+QLmRnGJpP35CkZGcY2kwtkcPitbNIl6SA7wtgs8hAWSc81mK2JYZ0cfAJzXHYGkNvkG8yGVNZWrso63wmLYlsYUyTekpdkTBiTHTm6JfyW5pDvsIPLAWXQRjcv9SWnySI3psv6BIt+1AKyE1YKUllbuSrjvKQLGIGkNywhv8lZ0ieMLYfduN9Y0VG0arA95Mw4Ny/JgW1IbKtE7pEHsD2FMsH3DqmMrUKVdd5LUb8DO5xvQFlSM1I2HE1PBKmM1CdiT5DklKKuCzhF9pATsIud4dallWWrLqXcZfImxTvyE5bC6bkD9ScT6fbvwtaq1tT8im5Yh9AlPUd2/SlFr5A1qfGYLcqs2FC1zxHyDNkXnmerUD2JvDQfVsvtsI3T0mG3kxdorFuveeQ1muej876ZSqpxZVG6zqU8W4XqqfODYFmgJpjVXFbC5ielxqN0OSdhpTMsNaeM+oqkT0RF53VeryJbhSrjvNK3HVa/Ps31nH+NRclxvduHh9+yrWfVNKMUUUW2Fua99PbQW6SGcs4X2SpUGefjpl2w9ZLW18K4T8O55CKaG9gxNF5cjO5VJG+LKP2+hOZI5qV9ka1ClXFeNfiZHEJS31NhzdI3IDmpGv+AxsapZ/Vp6qXPXjW0dBSjVEofkXzQ6OLyGl53tuopeAvNHb2o2+s7XtLGm2CptRuWwo9gjvpXTyyDLPSM19IwnndgXfJe8opsgL3qdMbZflFQd7b+iZTKi8kqWETSHx2tSM61ISmjPOnPkz6etK9e11kqa6tSpUqV/m/9BbUjz+IHhyAfAAAAAElFTkSuQmCC>