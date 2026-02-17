import torch

class TernaryQuantize(torch.autograd.Function):
    """
    Straight-Through Estimator (STE) for Ternary Quantization.
    Forward: 
        x > 0.5  -> +1
        x < -0.5 -> -1
        else     ->  0
    Backward:
        Gradient passed through unchanged (Identity).
        Optional: Clip gradient for x outside [-1, 1] to clamp? 
        We use pure Identity for simplicity.
    """
    
    @staticmethod
    def forward(ctx, input):
        # Quantize to {-1, 0, 1}
        # Thresholds: 0.5 and -0.5
        out = torch.zeros_like(input)
        out[input > 0.5] = 1.0
        out[input < -0.5] = -1.0
        return out

    @staticmethod
    def backward(ctx, grad_output):
        # Straight-Through Estimator
        # Just pass the gradient back as-is
        return grad_output, None

def quantize(x):
    return TernaryQuantize.apply(x)
