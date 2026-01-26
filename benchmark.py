"""
Helix-9 Comprehensive Benchmark
Tests both Dense and Sparse modes with the same dataset.
"""
import sys
import time
from helix9 import Helix9

# --- Encoding ---
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

def run_test(name, density, learning_rate, epochs=10000):
    """Run a single training test and return results."""
    print(f"\n{'='*60}")
    print(f"TEST: {name}")
    print(f"{'='*60}")
    
    helix = Helix9()
    helix.create_ai()
    
    CONTEXT_SIZE = 5
    INPUT_SIZE = CONTEXT_SIZE * 3
    HIDDEN_SIZE = 40 
    OUTPUT_SIZE = 3
    
    sparsity = (1 - density) * 100
    print(f"Architecture: {INPUT_SIZE} -> {HIDDEN_SIZE} ({density*100:.0f}% density, {sparsity:.0f}% sparsity) -> {OUTPUT_SIZE}")
    print(f"Learning Rate: {learning_rate}")
    
    helix.add_layer(INPUT_SIZE, HIDDEN_SIZE, density)
    helix.add_layer(HIDDEN_SIZE, OUTPUT_SIZE, 1.0)
    helix.set_learning_rate(learning_rate)
    
    # Training Data
    train_text = "hello world this is helix learning to speak " * 5
    inputs, targets = prepare_data(train_text, CONTEXT_SIZE)
    
    print(f"Training on {len(inputs)} samples for {epochs} epochs...")
    start_time = time.time()
    
    losses = []
    for epoch in range(epochs):
        # Batch training
        for i in range(len(inputs)):
            helix.train_step(inputs[i], targets[i])
        helix.apply_updates()
        
        # Record loss every 500 epochs
        if epoch % 500 == 0:
            total_loss = 0
            for i in range(len(inputs)):
                out = helix.predict(inputs[i], OUTPUT_SIZE)
                for j in range(OUTPUT_SIZE):
                    diff = out[j] - targets[i][j]
                    total_loss += diff * diff
            losses.append((epoch, total_loss))
            print(f"  Epoch {epoch}: Loss = {total_loss:.2f}")
    
    training_time = time.time() - start_time
    
    # Final loss
    final_loss = 0
    for i in range(len(inputs)):
        out = helix.predict(inputs[i], OUTPUT_SIZE)
        for j in range(OUTPUT_SIZE):
            diff = out[j] - targets[i][j]
            final_loss += diff * diff
    
    # Generation test
    seed = "hello"
    current_text = seed
    for _ in range(30):
        context = current_text[-CONTEXT_SIZE:]
        in_vec = []
        for c in context:
            in_vec.extend(encode_char(c))
        out_vec = helix.predict(in_vec, OUTPUT_SIZE)
        next_char = decode_trits(out_vec)
        current_text += next_char
    
    generated = current_text[len(seed):]
    
    return {
        'name': name,
        'density': density,
        'lr': learning_rate,
        'time': training_time,
        'final_loss': final_loss,
        'generated': generated,
        'passed': final_loss < 50
    }

def main():
    print("="*60)
    print("HELIX-9 COMPREHENSIVE BENCHMARK")
    print("="*60)
    print("Comparing Dense vs Sparse Training")
    
    results = []
    
    # Test 1: Dense Mode (100%)
    results.append(run_test(
        name="Dense (100%)",
        density=1.0,
        learning_rate=0.0005  # Reduced to prevent divergence
    ))
    
    # Test 2: Sparse Mode (80%)
    results.append(run_test(
        name="Sparse (80%)",
        density=0.8,
        learning_rate=0.0005  # Reduced to match
    ))
    
    # Test 3: Sparse Mode (50%)
    results.append(run_test(
        name="Sparse (50%)",
        density=0.5,
        learning_rate=0.0005
    ))
    
    # Summary Table
    print("\n" + "="*60)
    print("SUMMARY")
    print("="*60)
    print(f"{'Test':<20} {'Density':<10} {'Time':<10} {'Loss':<12} {'Status'}")
    print("-"*60)
    
    for r in results:
        status = "✓ PASS" if r['passed'] else "✗ FAIL"
        print(f"{r['name']:<20} {r['density']*100:>6.0f}%    {r['time']:>6.1f}s    {r['final_loss']:>8.2f}    {status}")
    
    print("\n" + "-"*60)
    print("GENERATED TEXT SAMPLES:")
    print("-"*60)
    for r in results:
        print(f"{r['name']}: \"{r['generated'][:40]}...\"")
    
    print("\n" + "="*60)
    all_passed = all(r['passed'] for r in results)
    if all_passed:
        print("ALL TESTS PASSED!")
    else:
        passed = sum(1 for r in results if r['passed'])
        print(f"{passed}/{len(results)} TESTS PASSED")
    print("="*60)

if __name__ == "__main__":
    main()
