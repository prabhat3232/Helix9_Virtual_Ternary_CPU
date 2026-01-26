from helix9 import HelixCPU, HelixAI
import time

def test_cpu():
    print("--- Testing Virtual CPU via Python ---")
    cpu = HelixCPU()
    
    print(f"Initial PC: {cpu.pc}")
    
    # Write Instruction: LDT R0, 50 (Op: 1, Reg: 0, Val: 50 -> encoded manually for PoC)
    # Actually, let's just test memory r/w which is easier than manual opcode encoding without assembler
    print("Writing Memory[100] = 42...")
    cpu.write_mem(100, 42)
    
    val = cpu.read_mem(100)
    print(f"Read Memory[100] = {val}")
    
    if val == 42:
        print("[PASS] CPU Memory R/W")
    else:
        print("[FAIL] CPU Memory R/W")

def test_ai():
    print("\n--- Testing Sparse AI via Python ---")
    ai = HelixAI()
    
    # Architecture: 2 Inputs -> 4 Hidden -> 1 Output
    ai.add_layer(2, 4, 1.0)
    ai.add_layer(4, 1, 1.0)
    ai.set_lr(0.1)
    
    # Train XOR
    print("Training XOR (1000 epochs)...")
    inputs = [
        [0.0, 0.0],
        [0.0, 1.0],
        [1.0, 0.0],
        [1.0, 1.0]
    ]
    targets = [
        [0.0],
        [1.0],
        [1.0],
        [0.0]
    ]
    
    start = time.time()
    for episode in range(1000):
        for i in range(4):
            ai.train(inputs[i], targets[i])
            
    end = time.time()
    print(f"Training Time: {end - start:.4f}s")
    
    # Predict
    print("Inference Results:")
    for i in range(4):
        out = ai.predict(inputs[i])
        print(f"In: {inputs[i]} -> Out: {out[0]:.4f}")

if __name__ == "__main__":
    test_cpu()
    test_ai()
