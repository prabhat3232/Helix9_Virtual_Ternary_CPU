import torch
import torch.nn as nn
import torch.optim as optim
import time
import sys
import psutil
import os

# --- 1. Encoding (Same as Helix-9) ---
CHARS = " abcdefghijklmnopqrstuvwxyz"
CHAR_TO_INT = {c: i for i, c in enumerate(CHARS)}
INT_TO_CHAR = {i: c for i, c in enumerate(CHARS)}

def encode_char(char):
    """Encodes a character into 3 ternary values (-1, 0, 1)."""
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
    """Decodes 3 ternary floats back to a char."""
    def unmap_trit(t):
        if t < -0.33: return 0
        if t > 0.33: return 2
        return 1
        
    d1 = unmap_trit(trits[0])
    d2 = unmap_trit(trits[1])
    d3 = unmap_trit(trits[2])
    
    val = d1*9 + d2*3 + d3
    return INT_TO_CHAR.get(val, '?')

# --- 2. Data Preparation (Same as Helix-9) ---
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

# --- 3. PyTorch Model (Same Architecture as Helix-9) ---
class LanguageModel(nn.Module):
    def __init__(self, input_size=15, hidden_size=40, output_size=3):
        super().__init__()
        self.fc1 = nn.Linear(input_size, hidden_size)
        self.fc2 = nn.Linear(hidden_size, output_size)
        self.relu = nn.ReLU()
        
    def forward(self, x):
        x = self.relu(self.fc1(x))
        x = self.fc2(x)
        return x

# --- 4. Metrics Collection ---
def get_memory_mb():
    """Get current process memory in MB."""
    process = psutil.Process(os.getpid())
    return process.memory_info().rss / (1024 * 1024)

def count_parameters(model):
    """Count trainable parameters."""
    return sum(p.numel() for p in model.parameters() if p.requires_grad)

# --- 5. Main Demo ---
def run_demo():
    print("--- PyTorch Baseline: Language Model ---")
    
    # Architecture (Same as Helix-9)
    CONTEXT_SIZE = 5
    INPUT_SIZE = CONTEXT_SIZE * 3
    HIDDEN_SIZE = 40
    OUTPUT_SIZE = 3
    
    print(f"Initializing Network: {INPUT_SIZE} -> {HIDDEN_SIZE} -> {OUTPUT_SIZE}")
    
    model = LanguageModel(INPUT_SIZE, HIDDEN_SIZE, OUTPUT_SIZE)
    criterion = nn.MSELoss()
    optimizer = optim.SGD(model.parameters(), lr=0.05, momentum=0.9)
    
    # Parameter Count
    param_count = count_parameters(model)
    print(f"Parameters: {param_count}")
    
    # Training Data (Same as Helix-9)
    train_text = "hello world this is helix learning to speak " * 5
    inputs, targets = prepare_data(train_text, CONTEXT_SIZE)
    
    # Convert to tensors
    X = torch.tensor(inputs, dtype=torch.float32)
    Y = torch.tensor(targets, dtype=torch.float32)
    
    print(f"Training on {len(inputs)} samples...")
    
    # Memory before training
    mem_before = get_memory_mb()
    start_time = time.time()
    
    EPOCHS = 3000
    for epoch in range(EPOCHS):
        optimizer.zero_grad()
        outputs = model(X)
        loss = criterion(outputs, Y)
        loss.backward()
        optimizer.step()
        
        if epoch % 100 == 0:
            print(f"Epoch {epoch}: Loss = {loss.item():.4f}")
    
    training_time = time.time() - start_time
    mem_after = get_memory_mb()
    final_loss = loss.item()
    
    print(f"\nTraining Complete in {training_time:.2f}s")
    
    # --- Results Summary ---
    print("\n" + "="*50)
    print("BENCHMARK RESULTS (PyTorch Float32)")
    print("="*50)
    print(f"Parameters:     {param_count}")
    print(f"Memory (MB):    {mem_after:.2f}")
    print(f"Training Time:  {training_time:.2f}s")
    print(f"Final Loss:     {final_loss:.4f}")
    print("="*50)
    
    # --- Generation Test ---
    seed = "hello"
    print(f"\nGenerative Test (Seed: '{seed}'):")
    current_text = seed
    
    model.eval()
    with torch.no_grad():
        for _ in range(50):
            context = current_text[-CONTEXT_SIZE:]
            in_vec = []
            for c in context:
                in_vec.extend(encode_char(c))
            
            x = torch.tensor([in_vec], dtype=torch.float32)
            out = model(x)
            
            next_char = decode_trits(out[0].tolist())
            current_text += next_char
            
            sys.stdout.write(next_char)
            sys.stdout.flush()
            time.sleep(0.02)
    
    print("\n\nDone.")

if __name__ == "__main__":
    run_demo()
