import torch
import torch.nn as nn
import torch.optim as optim
import os
import sys

# Add parent directory to path to import ternary modules
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'python'))
try:
    from ternary_layers import TernaryLinear
    from ternary_ops import quantize
except ImportError:
    print("Error: Could not import ternary_layers/ops. Make sure they exist in experiments/python/")
    sys.exit(1)

# Configuration
HIDDEN_SIZE = 16 # Small hidden state for demo
LEARNING_RATE = 0.1
EPOCHS = 200

# Data: "hello world"
text = "hello world"
chars = sorted(list(set(text)))
char_to_ix = { ch:i for i,ch in enumerate(chars) }
ix_to_char = { i:ch for i,ch in enumerate(chars) }
vocab_size = len(chars)

print(f"Vocab: {chars}")
print(f"Vocab Size: {vocab_size}")

# Prepare Data (Single sequence batch)
# Input: "h" -> Target: "e"
# Input: "e" -> Target: "l"
# ...
inputs = [char_to_ix[ch] for ch in text[:-1]]
targets = [char_to_ix[ch] for ch in text[1:]]

inputs_tensor = torch.tensor(inputs, dtype=torch.long)
targets_tensor = torch.tensor(targets, dtype=torch.long)

# One-Hot Encoding function
def to_one_hot(indices, vocab_size):
    batch_size = len(indices)
    one_hot = torch.zeros(batch_size, vocab_size)
    one_hot[torch.arange(batch_size), indices] = 1
    return one_hot

# Model: simple RNN
class TernaryRNN(nn.Module):
    def __init__(self, input_size, hidden_size, output_size):
        super(TernaryRNN, self).__init__()
        self.hidden_size = hidden_size
        
        # W_ih: Input -> Hidden
        self.i2h = TernaryLinear(input_size, hidden_size, bias=False)
        # W_hh: Hidden -> Hidden
        self.h2h = TernaryLinear(hidden_size, hidden_size, bias=False)
        # W_ho: Hidden -> Output
        self.h2o = TernaryLinear(hidden_size, output_size, bias=False)
        
    def forward(self, input, hidden):
        # Update Hidden: h_new = quantized( W_ih * x + W_hh * h_prev )
        # Note: In standard RNN, activation is Tanh. 
        # In Ternary Helix Logic: Activation is essentially 'Quantize' or 'Clamp'
        
        i2h_out = self.i2h(input)
        h2h_out = self.h2h(hidden)
        
        # Combine (Simple Add)
        pre_act = i2h_out + h2h_out
        
        # Activation (Quantize to -1, 0, 1)
        hidden_new = quantize(pre_act)
        
        # Output layer
        output = self.h2o(hidden_new)
        return output, hidden_new

    def init_hidden(self):
        return torch.zeros(1, self.hidden_size)

model = TernaryRNN(vocab_size, HIDDEN_SIZE, vocab_size)
optimizer = optim.Adam(model.parameters(), lr=LEARNING_RATE)
loss_fn = nn.CrossEntropyLoss()

print("\n--- Training ---")
for epoch in range(EPOCHS):
    total_loss = 0
    hidden = model.init_hidden()
    
    optimizer.zero_grad()
    loss = 0
    
    # Unroll sequence
    input_one_hot = to_one_hot(inputs_tensor, vocab_size)
    
    for i in range(len(inputs)):
        inp = input_one_hot[i].unsqueeze(0) # Batch size 1
        target = targets_tensor[i].unsqueeze(0)
        
        output, hidden = model(inp, hidden)
        loss += loss_fn(output, target)

    loss.backward()
    optimizer.step()
    
    # Clamp weights
    for p in model.parameters():
        p.data.clamp_(-1.5, 1.5)
        
    if epoch % 20 == 0:
        print(f"Epoch {epoch}: Loss {loss.item():.4f}")

# Test
print("\n--- Testing ---")
with torch.no_grad():
    hidden = model.init_hidden()
    test_input = inputs_tensor[0] # 'h'
    start_char = ix_to_char[test_input.item()]
    print(f"Seed: '{start_char}'")
    
    generated = start_char
    
    inp = to_one_hot(torch.tensor([test_input]), vocab_size)
    
    for _ in range(len(text)-1):
        output, hidden = model(inp, hidden)
        
        # Argmax
        pred_ix = output.argmax(dim=1).item()
        generated += ix_to_char[pred_ix]
        
        # Next input
        inp = to_one_hot(torch.tensor([pred_ix]), vocab_size)
        
    print(f"Generated: '{generated}'")
    if generated == text:
        print("SUCCESS: Perfect reproduction.")
    else:
        print("PARTIAL: Reproduction imperfect.")

# Export Weights
print("\n--- Exporting to Helix .DAT ---")

def export_matrix(name, tensor, f):
    # Tensor shape: [Rows, Cols]
    # Helix: Row-major
    rows, cols = tensor.shape
    f.write(f"; Matrix {name} [{rows}x{cols}]\n")
    f.write(f"{name}: .DAT ")
    
    data = tensor.detach().round().int().tolist()
    flat_data = []
    for row in data:
        flat_data.extend(row)
        
    # Write in chunks of 16 for readability
    for i, val in enumerate(flat_data):
        f.write(f"{val} ")
        if (i+1) % 16 == 0 and (i+1) < len(flat_data):
            f.write("\n.DAT ")
    f.write("\n\n")

os.makedirs("experiments/nlp", exist_ok=True)
with open("experiments/nlp/rnn_weights.dat", "w") as f:
    f.write("; RNN Weights for Helix9\n")
    f.write(f"; Vocab Size: {vocab_size}\n")
    f.write(f"; Hidden Size: {HIDDEN_SIZE}\n\n")
    
    # Map chars to indices (for C++ lookup)
    f.write("; Vocabulary Mapping:\n")
    for i, ch in enumerate(chars):
        f.write(f"; {i} -> '{ch}'\n")
    f.write("\n")
    
    export_matrix("W_ih", model.i2h.weight, f) # [Hidden, Input]
    export_matrix("W_hh", model.h2h.weight, f) # [Hidden, Hidden]
    export_matrix("W_ho", model.h2o.weight, f) # [Output, Hidden]

print("Weights exported to 'experiments/nlp/rnn_weights.dat'")
