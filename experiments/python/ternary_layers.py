import torch
import torch.nn as nn
import torch.nn.functional as F
from ternary_ops import quantize

class TernaryLinear(nn.Module):
    """
    Linear layer where Weights and Inputs are quantized to {-1, 0, 1}.
    Simulates the Helix9 'SAT.MAC' instruction.
    """
    def __init__(self, in_features, out_features, bias=False):
        super(TernaryLinear, self).__init__()
        self.in_features = in_features
        self.out_features = out_features
        
        # Weights are stored in float for training, but quantized for forward pass
        self.weight = nn.Parameter(torch.Tensor(out_features, in_features))
        if bias:
            self.bias = nn.Parameter(torch.Tensor(out_features))
        else:
            self.register_parameter('bias', None)
            
        self.reset_parameters()

    def reset_parameters(self):
        # Initialize with wider range to ensure some weights start at -1 or 1
        # STE needs non-zero gradients, which pass through regardless of quantization,
        # but starting with non-zero quantized values helps signal propagation.
        # Uniform [-1.5, 1.5] covers -1, 0, 1 well.
        nn.init.uniform_(self.weight, -1.5, 1.5)
        if self.bias is not None:
            nn.init.uniform_(self.bias, -1.0, 1.0)

    def forward(self, input):
        # 1. Quantize Weights
        w_q = quantize(self.weight)
        
        # 2. Quantize Input (Ensure interactions are Integer instructions)
        # Note: If input is already quantized (e.g. previous layer output), this is idempotent.
        x_q = quantize(input)
        
        # 3. Compute Linear Operation
        # Corresponds to Helix9 VEC.SAT.MAC
        return F.linear(x_q, w_q, self.bias)

    def extra_repr(self):
        return 'in_features={}, out_features={}, bias={}'.format(
            self.in_features, self.out_features, self.bias is not None
        )
