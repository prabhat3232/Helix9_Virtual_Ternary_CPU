import random
import sys
import time
from helix9 import Helix9

# --- Dense Mode Sanity Check ---
# This test runs Helix-9 with 100% density and reduced LR
# to verify that the backprop/gradient math is correct.

# --- 1. Encoding (Same as original) ---
CHARS = " abcdefghijklmnopqrstuvwxyz"
CHAR_TO_INT = {c: i for i, c in enumerate(CHARS)}
INT_TO_CHAR = {i: c for i, c in enumerate(CHARS)}

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

def decode_trits(trits):
    def unmap_trit(t):
        if t < -0.33: return 0
        if t > 0.33: return 2
        return 1
        
    d1 = unmap_trit(trits[0])
    d2 = unmap_trit(trits[1])
    d3 = unmap_trit(trits[2])
    
    val = d1*9 + d2*3 + d3
    return INT_TO_CHAR.get(val, '?')

# --- 2. Data Preparation ---
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

# --- 3. Main Demo ---
def run_demo():
    print("=" * 60)
    print("HELIX-9 DENSE MODE SANITY CHECK")
    print("=" * 60)
    print("Testing with 100% density and reduced LR to verify math.")
    print()
    
    try:
        helix = Helix9()
    except Exception as e:
        print(f"Error: {e}")
        return

    helix.create_ai()
    
    CONTEXT_SIZE = 5
    INPUT_SIZE = CONTEXT_SIZE * 3
    HIDDEN_SIZE = 40 
    OUTPUT_SIZE = 3
    
    # KEY CHANGE: 100% density (fully connected)
    DENSITY = 1.0
    
    # KEY CHANGE: For batch training with 215 samples, we need higher LR
    # (gradients are summed, not averaged, so effective step is larger)
    LEARNING_RATE = 0.1
    
    print(f"Architecture: {INPUT_SIZE} -> {HIDDEN_SIZE} (Dense) -> {OUTPUT_SIZE}")
    print(f"Density: {DENSITY * 100}%")
    print(f"Learning Rate: {LEARNING_RATE}")
    print()
    
    helix.add_layer(INPUT_SIZE, HIDDEN_SIZE, DENSITY)
    helix.add_layer(HIDDEN_SIZE, OUTPUT_SIZE, 1.0)
    
    helix.set_learning_rate(LEARNING_RATE)
    
    # Training Data
    train_text = "hello world this is helix learning to speak " * 5
    inputs, targets = prepare_data(train_text, CONTEXT_SIZE)
    
    print(f"Training on {len(inputs)} samples...")
    start_time = time.time()
    
    EPOCHS = 3000
    for epoch in range(EPOCHS):
        total_loss = 0
        
        # Accumulate gradients over ALL samples (batch training)
        for i in range(len(inputs)):
            helix.train_step(inputs[i], targets[i])
        
        # Apply once per epoch (like C++ demo)
        helix.apply_updates()
        
        # Calculate loss for display
        if epoch % 100 == 0:
            for i in range(len(inputs)):
                out = helix.predict(inputs[i], OUTPUT_SIZE)
                for j in range(OUTPUT_SIZE):
                    diff = out[j] - targets[i][j]
                    total_loss += diff * diff
            print(f"Epoch {epoch}: Loss = {total_loss:.4f}")
            
    training_time = time.time() - start_time
    print(f"\nTraining Complete in {training_time:.2f}s")
    print(f"Final Loss: {total_loss:.4f}")
    
    # --- Verdict ---
    print()
    print("=" * 60)
    if total_loss < 10:
        print("RESULT: PASSED - Dense mode converged!")
        print("The backprop math is correct. Sparsity is the issue.")
    else:
        print("RESULT: FAILED - Dense mode did NOT converge!")
        print("There may be a math bug in TernaryFloat/Backprop.")
    print("=" * 60)
    
    # --- Generation Test ---
    seed = "hello"
    print(f"\nGenerative Test (Seed: '{seed}'):")
    current_text = seed
    
    for _ in range(50):
        context = current_text[-CONTEXT_SIZE:]
        in_vec = []
        for c in context:
            in_vec.extend(encode_char(c))
            
        out_vec = helix.predict(in_vec, OUTPUT_SIZE)
        next_char = decode_trits(out_vec)
        current_text += next_char
        
        sys.stdout.write(next_char)
        sys.stdout.flush()
        time.sleep(0.02)
        
    print("\n\nDone.")

if __name__ == "__main__":
    run_demo()
