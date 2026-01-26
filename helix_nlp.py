import numpy as np

# Alphabet: Space + a-z (27 chars)
CHARS = " abcdefghijklmnopqrstuvwxyz"
CHAR_TO_IDX = {c: i for i, c in enumerate(CHARS)}
IDX_TO_CHAR = {i: c for i, c in enumerate(CHARS)}

def char_to_trits(char):
    """
    Encodes a character into 3 Trits.
    Mapping: 0..26 -> Base 3 -> Trits (-1, 0, 1)
    """
    char = char.lower()
    if char not in CHAR_TO_IDX:
        # Default to space for unknown chars
        idx = 0 
    else:
        idx = CHAR_TO_IDX[char]
        
    # Decompose into 3 digits of base 3
    # value = d2*9 + d1*3 + d0
    d0 = idx % 3
    idx //= 3
    d1 = idx % 3
    d2 = idx // 3
    
    # Map 0,1,2 -> -1.0, 0.0, 1.0 (Balanced Ternary-ish)
    # Actually, let's map: 0 -> -1, 1 -> 0, 2 -> 1
    t0 = float(d0 - 1)
    t1 = float(d1 - 1)
    t2 = float(d2 - 1)
    
    return [t2, t1, t0]

def trits_to_char(trits):
    """
    Decodes 3 Trits into a character.
    trits: list of 3 floats
    """
    # Round to nearest integer (-1, 0, 1)
    def clean(x):
        if x < -0.5: return 0 # was -1
        if x > 0.5: return 2  # was 1
        return 1              # was 0
        
    d2 = clean(trits[0])
    d1 = clean(trits[1])
    d0 = clean(trits[2])
    
    idx = d2*9 + d1*3 + d0
    
    # Clamp to valid range
    idx = max(0, min(26, idx))
    
    return IDX_TO_CHAR[idx]

def text_to_tensor(text, context_window):
    """
    Prepares X (Inputs) and Y (Targets) for training.
    X: Flattened list of trits for 'context_window' chars.
    Y: One-hot encoding of the NEXT character (size 27).
       Note: We use One-Hot for output target to use Softmax/CrossEntropy logic 
       (or just MSE on the 1-hot vector) for better convergence than regressing 3 trits directly.
    """
    X = []
    Y = []
    
    clean_text = "".join([c for c in text.lower() if c in CHARS])
    
    for i in range(len(clean_text) - context_window):
        window = clean_text[i : i+context_window]
        target = clean_text[i + context_window]
        
        # Encode Window -> Trits
        x_seq = []
        for c in window:
            x_seq.extend(char_to_trits(c))
        X.append(x_seq)
        
        # Encode Target -> One-Hot (Size 27)
        # Why One-Hot? Ternary regression for classification is tricky with simple MSE.
        # But wait, our usage of "helix9.py" calls `train` with float arrays.
        # If we use 3 Trits as output, we are forcing the AI to learn the binary-coded-decimal equivalent.
        # That adds unnecessary complexity to the learning task.
        # It is standard to use One-Hot for Char-RNNs.
        # But our AI is sparse ternary...
        # Let's try predicting the 3 Trits directly! It's more "Ternary Native".
        Y.append(char_to_trits(target))
        
    return X, Y

def trits_to_onehot(char_trits):
    # Not used if we output trits directly
    pass
