import torch
import torch.nn as nn
import torch.optim as optim
from ternary_layers import TernaryLinear
from ternary_ops import quantize

# 1. Dataset: XOR with Bias Column (Input 3 = 1)
# Inputs -> Labels
X = torch.tensor([
    [-1., -1., 1.], 
    [-1.,  1., 1.], 
    [ 1., -1., 1.], 
    [ 1.,  1., 1.]
])
Y = torch.tensor([[-1.], [1.], [1.], [-1.]])

# 2. Model: MLP
# 3 Inputs (2 + Bias) -> 4 Hidden -> 1 Output
class XORModel(nn.Module):
    def __init__(self):
        super(XORModel, self).__init__()
        # Layer 1: 3 inputs (Bias included)
        self.fc1 = TernaryLinear(3, 8, bias=False) # Increased width to 8
        # Layer 2: 8 inputs
        self.fc2 = TernaryLinear(8, 1, bias=False)

    def forward(self, x):
        h = self.fc1(x)
        # Activation: Quantize (-1, 0, 1)
        h = quantize(h) 
        # Add Bias to hidden layer? 
        # Ideally Layer 2 also needs bias.
        # We can append column of 1s to h?
        # For simplicity, let's try without bias in layer 2 first, or assume sufficient width handles it.
        # Reference: XOR usually needs bias in hidden.
        out = self.fc2(h)
        return out

model = XORModel()
optimizer = optim.Adam(model.parameters(), lr=0.05) # Lower LR slightly
criterion = nn.MSELoss() 

print("Training XOR on Ternary Network...")
for epoch in range(1000): # More epochs
    optimizer.zero_grad()
    outputs = model(X)
    loss = criterion(outputs, Y)
    loss.backward()
    optimizer.step()
    
    # Textual clamp (Project Weights) to keep them from growing too large
    # This helps Quantization Aware Training (QAT)
    for p in model.parameters():
        p.data.clamp_(-1.5, 1.5)

    if epoch % 100 == 0:
        print(f"Epoch {epoch}: Loss = {loss.item()}")

# Testing
print("\nResults:")
with torch.no_grad():
    preds = model(X)
    for i in range(4):
        # We look at the sign of the output
        p_val = preds[i].item()
        p_lbl = 1 if p_val > 0 else -1
        truth = Y[i].item()
        print(f"In: {X[i].numpy()} -> Pred: {p_val:.2f} ({p_lbl}) vs Truth: {truth} [{'PASS' if p_lbl==truth else 'FAIL'}]")

# Dump Weights for Helix9 Reference
print("\nTernary Weights (Layer 1):")
print(quantize(model.fc1.weight).int())
print("\nTernary Weights (Layer 2):")
print(quantize(model.fc2.weight).int())
