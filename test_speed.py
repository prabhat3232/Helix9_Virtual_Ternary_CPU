"""
Speed Comparison: Python Loop vs C++ Fit
"""
import time
from helix9 import Helix9

# --- Encoding ---
CHARS = " abcdefghijklmnopqrstuvwxyz"
CHAR_TO_INT = {c: i for i, c in enumerate(CHARS)}

def encode_char(char):
    if char not in CHAR_TO_INT:
        char = ' '
    val = CHAR_TO_INT[char]
    d1 = val // 9
    rem1 = val % 9
    d2 = rem1 // 3
    d3 = rem1 % 3
    
    def map_trit(d):
        if d == 0: return -1.0
        if d == 1: return 0.0
        if d == 2: return 1.0
        return 0.0

    return [map_trit(d1), map_trit(d2), map_trit(d3)]

def prepare_data(text, context_size=5):
    inputs = []
    targets = []
    
    for i in range(len(text) - context_size):
        window = text[i : i+context_size]
        next_char = text[i+context_size]
        
        in_vec = []
        for c in window:
            in_vec.extend(encode_char(c))
            
        tgt_vec = encode_char(next_char)
        
        inputs.append(in_vec)
        targets.append(tgt_vec)
        
    return inputs, targets

def main():
    print("=" * 60)
    print("SPEED COMPARISON: Python Loop vs C++ Fit")
    print("=" * 60)
    
    # Prepare data
    train_text = "hello world this is helix learning to speak " * 5
    inputs, targets = prepare_data(train_text, 5)
    
    EPOCHS = 5000
    LR = 0.0005
    
    print(f"Training data: {len(inputs)} samples")
    print(f"Epochs: {EPOCHS}")
    print()
    
    # --- Test 1: Python Loop (old method) ---
    print("Method 1: Python Loop (train_step + apply_updates)")
    helix1 = Helix9()
    helix1.create_ai()
    helix1.add_layer(15, 40, 1.0)
    helix1.add_layer(40, 3, 1.0)
    helix1.set_learning_rate(LR)
    
    start1 = time.time()
    for epoch in range(EPOCHS):
        for i in range(len(inputs)):
            helix1.train_step(inputs[i], targets[i])
        helix1.apply_updates()
    time1 = time.time() - start1
    
    # Calculate final loss
    loss1 = 0
    for i in range(len(inputs)):
        out = helix1.predict(inputs[i], 3)
        for j in range(3):
            diff = out[j] - targets[i][j]
            loss1 += diff * diff
    
    print(f"  Time: {time1:.2f}s")
    print(f"  Loss: {loss1:.4f}")
    print()
    
    # --- Test 2: C++ Fit (new method) ---
    print("Method 2: C++ Fit (entire loop in C++)")
    helix2 = Helix9()
    helix2.create_ai()
    helix2.add_layer(15, 40, 1.0)
    helix2.add_layer(40, 3, 1.0)
    helix2.set_learning_rate(LR)
    
    start2 = time.time()
    loss2 = helix2.fit(inputs, targets, EPOCHS, log_interval=0)
    time2 = time.time() - start2
    
    print(f"  Time: {time2:.2f}s")
    print(f"  Loss: {loss2:.4f}")
    print()
    
    # --- Summary ---
    print("=" * 60)
    print("SUMMARY")
    print("=" * 60)
    print(f"{'Method':<25} {'Time':<10} {'Loss':<12} {'Speedup'}")
    print("-" * 60)
    speedup = time1 / time2 if time2 > 0 else 0
    print(f"{'Python Loop':<25} {time1:>6.2f}s    {loss1:>8.4f}")
    print(f"{'C++ Fit':<25} {time2:>6.2f}s    {loss2:>8.4f}    {speedup:.1f}x faster")
    print("=" * 60)

if __name__ == "__main__":
    main()
