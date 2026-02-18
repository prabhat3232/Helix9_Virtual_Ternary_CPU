import torch
import torch.nn as nn
from mnist_ternary import TernaryMNIST  # Ensure this file exists in the same directory or adjust imports
from ternary_ops import quantize
import os
import sys

# Add parent directory to path to locate dependencies if running from experiments/python
sys.path.append(os.path.dirname(os.path.abspath(__file__)))


# Load Model
model = TernaryMNIST()
MODEL_PATH = "experiments/python/mnist_ternary.pth"
if not os.path.exists(MODEL_PATH):
    # Try local path
    MODEL_PATH = "mnist_ternary.pth"
    if not os.path.exists(MODEL_PATH):
        print(f"Error: Could not find model at {MODEL_PATH}")
        # Generating random weights for demo purposes if model not found
        print("Using random weights for export...")
    else:
        try:
            model.load_state_dict(torch.load(MODEL_PATH))
            print(f"Loaded model from {MODEL_PATH}")
        except Exception as e:
            print(f"Error loading model: {e}")
            print("Using random weights...")

model.eval()

# Reduce size for demo
# Slicing the 784->512->10 model to 784->32->10 for Proof of Concept performance.
HIDDEN_SIZE = 32

def export_to_py(filepath):
    print(f"Exporting weights to {filepath} (Hidden Size: {HIDDEN_SIZE})")
    
    # Layer 1: 784 -> 512 (Slice to 784 -> HIDDEN_SIZE)
    # Weight Shape: [Out, In] -> [512, 784]
    w1 = quantize(model.fc1.weight).int().cpu().numpy()[:HIDDEN_SIZE, :]
    b1 = model.fc1.bias.data.int().cpu().numpy()[:HIDDEN_SIZE]
    
    # Layer 2: 512 -> 10 (Slice to 32 -> 10)
    # Weight Shape: [10, 512] -> [10, 32]
    # We must only take the columns corresponding to the kept hidden neurons.
    w2 = quantize(model.fc2.weight).int().cpu().numpy()[:, :HIDDEN_SIZE]
    b2 = model.fc2.bias.data.int().cpu().numpy()
    
    with open(filepath, "w") as f:
        f.write("# MNIST Weights (Ternary Quantized)\n")
        f.write(f"# Input: 784, Hidden: {HIDDEN_SIZE}, Output: 10\n\n")
        
        # W1 (Transposed for A @ W layout if A is 1x784? Or W @ A?)
        # PyTorch is y = xA^T + b.
        # Helix A @ B is standard row*col.
        # If Input X is [1, 784].
        # We want [1, 32] output.
        # So W1 should be [784, 32].
        # PyTorch W1 is [32, 784]. So we need to TRANSPOSE.
        
        f.write(f"W1_ROWS = 784\n")
        f.write(f"W1_COLS = {HIDDEN_SIZE}\n")
        
        # Flattened list for Helix Compiler 'fill' or direct usage
        # W1
        f.write("W1_DATA = [\n")
        w1_t = w1.T
        count = 0
        for r in range(w1_t.shape[0]):
            for val in w1_t[r]:
                f.write(str(val) + ", ")
                count += 1
                if count % 32 == 0: f.write("\n    ")
        f.write("]\n\n")
        
        # B1
        f.write(f"B1_COLS = {HIDDEN_SIZE}\n")
        f.write("B1_DATA = [" + ", ".join(map(str, b1)) + "]\n\n")
        
        # W2
        f.write(f"W2_ROWS = {HIDDEN_SIZE}\n")
        f.write("W2_COLS = 10\n")
        f.write("W2_DATA = [\n")
        w2_t = w2.T
        count = 0
        for r in range(w2_t.shape[0]):
            for val in w2_t[r]:
                f.write(str(val) + ", ")
                count += 1
                if count % 10 == 0: f.write("\n    ")
        f.write("]\n\n")
        
        # B2
        f.write("B2_COLS = 10\n")
        f.write("B2_DATA = [" + ", ".join(map(str, b2)) + "]\n")

if __name__ == "__main__":
    export_to_py("experiments/python/mnist_weights_data.py")
