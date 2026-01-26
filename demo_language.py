import random
import sys
import time
from helix9 import Helix9

# --- 1. Balanced Ternary Encoding ---
# We map 27 chars (a-z + space) to 3 Trits
# 3^3 = 27 unique combinations.
# Values: -1, 0, 1

CHARS = " abcdefghijklmnopqrstuvwxyz"
CHAR_TO_INT = {c: i for i, c in enumerate(CHARS)}
INT_TO_CHAR = {i: c for i, c in enumerate(CHARS)}

def encode_char(char):
    """Encodes a character into 3 ternary values (-1, 0, 1)."""
    if char not in CHAR_TO_INT:
        char = ' ' # Fallback
    
    val = CHAR_TO_INT[char]
    trits = []
    
    # Base 3 conversion (balanced)
    # Simple strategy: Convert to standard base 3 (0,1,2), then shift to (-1,0,1)?
    # Actually, standard balanced ternary is often simpler:
    # Let's just map index 0..26 to specific 3-trit patterns.
    # To keep it deterministic:
    # 0 -> (-1, -1, -1)
    # 1 -> (-1, -1,  0)
    # ...
    
    # Let's assume standard base 3 for 0-26:
    # 0 = 000_3
    # 26 = 222_3 (actually 222_3 = 2*9 + 2*3 + 2 = 18+6+2 = 26) Perfect!
    # So we map 0->-1, 1->0, 2->1 for "Balanced Values"
    
    # Decompose val into 3 digits of base 3
    d1 = val // 9
    rem1 = val % 9
    d2 = rem1 // 3
    d3 = rem1 % 3
    
    # Map 0,1,2 to -1,0,1
    def map_trit(d):
        if d == 0: return -1.0
        if d == 1: return 0.0
        if d == 2: return 1.0
        return 0.0

    return [map_trit(d1), map_trit(d2), map_trit(d3)]

def decode_trits(trits):
    """Decodes 3 ternary floats back to a char."""
    # We need to map real values back to integers 0,1,2
    # Thresholds: < -0.33 -> 0 (-1), > 0.33 -> 2 (1), else 1 (0)
    
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
    
    # Sliding window
    for i in range(len(text) - context_size):
        window = text[i : i+context_size]
        next_char = text[i+context_size]
        
        # Encode Input
        in_vec = []
        for c in window:
            in_vec.extend(encode_char(c))
            
        # Encode Target (Next char)
        # For regression training, we target the raw trit values
        tgt_vec = encode_char(next_char)
        
        inputs.append(in_vec)
        targets.append(tgt_vec)
        
    return inputs, targets

# --- 3. Main Demo ---
def custom_print(s):
    try:
        sys.stdout.write(s)
        sys.stdout.flush()
    except Exception:
        pass

def run_demo():
    print("--- Helix-9 Language Model Demo ---")
    
    try:
        helix = Helix9() # Auto-finds DLL
    except Exception as e:
        print(f"Error: {e}")
        return

    helix.create_ai()
    
    # Architecture
    # Input: 5 chars * 3 trits = 15 Inputs
    # Hidden: Let's try 30 Neurons (Sparse)
    # Output: 3 Neurons (Next char trits)
    
    CONTEXT_SIZE = 5
    INPUT_SIZE = CONTEXT_SIZE * 3
    HIDDEN_SIZE = 40 
    OUTPUT_SIZE = 3
    
    print(f"Initializing Network: {INPUT_SIZE} -> {HIDDEN_SIZE} (Sparse) -> {OUTPUT_SIZE}")
    helix.add_layer(INPUT_SIZE, HIDDEN_SIZE, 0.2) # 20% Sparsity
    helix.add_layer(HIDDEN_SIZE, OUTPUT_SIZE, 1.0) # Output dense
    
    helix.set_learning_rate(0.05)
    
    # Training Data
    # A simple repeating phrase to learn easily
    train_text = "hello world this is helix learning to speak " * 5
    inputs, targets = prepare_data(train_text, CONTEXT_SIZE)
    
    print(f"Training on {len(inputs)} samples...")
    start_time = time.time()
    
    EPOCHS = 3000
    for epoch in range(EPOCHS):
        total_loss = 0
        for i in range(len(inputs)):
            loss = helix.train(inputs[i], targets[i])
            total_loss += loss
            
        if epoch % 100 == 0:
            print(f"Epoch {epoch}: Loss = {total_loss:.4f}")
            
    print(f"Training Complete in {time.time() - start_time:.2f}s")
    
    # --- Generation ---
    seed = "hello"
    print(f"\nGenerative Test (Seed: '{seed}'):")
    current_text = seed
    
    generated_output = seed
    
    for _ in range(50): # Generate 50 chars
        # Encode last chars
        context = current_text[-CONTEXT_SIZE:]
        in_vec = []
        for c in context:
            in_vec.extend(encode_char(c))
            
        # Predit
        out_vec = helix.predict(in_vec, OUTPUT_SIZE)
        
        # Decode
        next_char = decode_trits(out_vec)
        generated_output += next_char
        current_text += next_char
        
        custom_print(next_char)
        time.sleep(0.05) # Typing effect
        
    print("\n\nDone.")

if __name__ == "__main__":
    run_demo()
