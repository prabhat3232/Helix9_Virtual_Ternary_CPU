import ctypes
import os
import sys

# Define proper ctypes types for clarity
c_double_p = ctypes.POINTER(ctypes.c_double)

class Helix9:
    def __init__(self, dll_path=None):
        if dll_path is None:
            # Default to finding it in the build directory relative to this script
            base_dir = os.path.dirname(os.path.abspath(__file__))
            # Try common paths
            possible_paths = [
                os.path.join(base_dir, "build", "Debug", "helix9_lib.dll"),
                os.path.join(base_dir, "build", "Release", "helix9_lib.dll"),
                os.path.join(base_dir, "build", "helix9_lib.dll"),
                os.path.join(base_dir, "helix9_lib.dll")
            ]
            
            for p in possible_paths:
                if os.path.exists(p):
                    dll_path = p
                    break
            
            if dll_path is None:
                raise FileNotFoundError("Could not find helix9_lib.dll. Please build the project or specify dll_path.")

        try:
            self.lib = ctypes.CDLL(dll_path)
        except OSError as e:
            print(f"Error loading DLL from {dll_path}: {e}")
            raise

        # --- CPU Bindings ---
        self.lib.Helix_CreateCPU.argtypes = []
        self.lib.Helix_CreateCPU.restype = None

        self.lib.Helix_DestroyCPU.argtypes = []
        self.lib.Helix_DestroyCPU.restype = None

        self.lib.Helix_CPU_WriteMem.argtypes = [ctypes.c_int, ctypes.c_int]
        self.lib.Helix_CPU_WriteMem.restype = None

        self.lib.Helix_CPU_ReadMem.argtypes = [ctypes.c_int]
        self.lib.Helix_CPU_ReadMem.restype = ctypes.c_int

        self.lib.Helix_CPU_Step.argtypes = []
        self.lib.Helix_CPU_Step.restype = None

        self.lib.Helix_CPU_GetPC.argtypes = []
        self.lib.Helix_CPU_GetPC.restype = ctypes.c_int

        self.lib.Helix_CPU_GetRegister.argtypes = [ctypes.c_int]
        self.lib.Helix_CPU_GetRegister.restype = ctypes.c_int

        # --- AI Bindings ---
        self.lib.Helix_CreateAI.argtypes = []
        self.lib.Helix_CreateAI.restype = None

        self.lib.Helix_AI_AddLayer.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_double]
        self.lib.Helix_AI_AddLayer.restype = None

        self.lib.Helix_AI_SetLR.argtypes = [ctypes.c_double]
        self.lib.Helix_AI_SetLR.restype = None

        self.lib.Helix_AI_Train.argtypes = [c_double_p, ctypes.c_int, c_double_p, ctypes.c_int]
        self.lib.Helix_AI_Train.restype = ctypes.c_double

        # NEW: Batch training support
        self.lib.Helix_AI_TrainStep.argtypes = [c_double_p, ctypes.c_int, c_double_p, ctypes.c_int]
        self.lib.Helix_AI_TrainStep.restype = None
        
        self.lib.Helix_AI_ApplyUpdates.argtypes = []
        self.lib.Helix_AI_ApplyUpdates.restype = None

        self.lib.Helix_AI_Predict.argtypes = [c_double_p, ctypes.c_int, c_double_p, ctypes.c_int]
        self.lib.Helix_AI_Predict.restype = None

        # PHASE 3: High-performance Fit function (entire training in C++)
        self.lib.Helix_AI_Fit.argtypes = [
            c_double_p, c_double_p,  # all_inputs, all_targets
            ctypes.c_int, ctypes.c_int, ctypes.c_int,  # num_samples, in_size, out_size
            ctypes.c_int, ctypes.c_int  # epochs, log_interval
        ]
        self.lib.Helix_AI_Fit.restype = ctypes.c_double

        # Auto-initialize CPU on create
        self.lib.Helix_CreateCPU()

    def __del__(self):
        # Clean up global state if this wrapper owns it purely.
        # Note: Since the C++ side uses globals, multiple Python instances 
        # would interfere. This is a known limitation of the current C++ design.
        if hasattr(self, 'lib'):
            self.lib.Helix_DestroyCPU()

    # --- CPU Methods ---
    def reset_cpu(self):
        self.lib.Helix_CreateCPU()

    def write_mem(self, addr, val):
        self.lib.Helix_CPU_WriteMem(addr, val)

    def read_mem(self, addr):
        return self.lib.Helix_CPU_ReadMem(addr)

    def step(self):
        self.lib.Helix_CPU_Step()

    def get_pc(self):
        return self.lib.Helix_CPU_GetPC()

    def get_register(self, index):
        return self.lib.Helix_CPU_GetRegister(index)

    # --- AI Methods ---
    def create_ai(self):
        self.lib.Helix_CreateAI()

    def add_layer(self, input_size, output_size, sparsity=1.0):
        self.lib.Helix_AI_AddLayer(input_size, output_size, sparsity)

    def set_learning_rate(self, lr):
        self.lib.Helix_AI_SetLR(lr)

    def train(self, inputs, targets):
        """
        Train the network on a single sample.
        inputs: list or list-like of floats
        targets: list or list-like of floats
        Returns: loss (float)
        """
        in_arr = (ctypes.c_double * len(inputs))(*inputs)
        tgt_arr = (ctypes.c_double * len(targets))(*targets)
        
        return self.lib.Helix_AI_Train(in_arr, len(inputs), tgt_arr, len(targets))

    def train_step(self, inputs, targets):
        """
        Accumulate gradients for a single sample without applying.
        Call apply_updates() after processing a batch.
        """
        in_arr = (ctypes.c_double * len(inputs))(*inputs)
        tgt_arr = (ctypes.c_double * len(targets))(*targets)
        self.lib.Helix_AI_TrainStep(in_arr, len(inputs), tgt_arr, len(targets))
    
    def apply_updates(self):
        """Apply accumulated gradients. Call after a batch of train_step() calls."""
        self.lib.Helix_AI_ApplyUpdates()

    def predict(self, inputs, output_size):
        """
        Run inference.
        inputs: list of floats
        output_size: expected number of outputs (must match last layer)
        Returns: list of floats
        """
        in_arr = (ctypes.c_double * len(inputs))(*inputs)
        out_arr = (ctypes.c_double * output_size)()
        
        self.lib.Helix_AI_Predict(in_arr, len(inputs), out_arr, output_size)
        
        return list(out_arr)

    def fit(self, inputs_list, targets_list, epochs, log_interval=500):
        """
        High-performance training - entire epoch loop runs in C++.
        
        inputs_list: list of input samples (each is a list of floats)
        targets_list: list of target samples (each is a list of floats)
        epochs: number of training epochs
        log_interval: print loss every N epochs (0 = no logging)
        
        Returns: final loss (float)
        """
        num_samples = len(inputs_list)
        in_size = len(inputs_list[0])
        out_size = len(targets_list[0])
        
        # Flatten inputs and targets
        flat_inputs = []
        for sample in inputs_list:
            flat_inputs.extend(sample)
        
        flat_targets = []
        for sample in targets_list:
            flat_targets.extend(sample)
        
        # Convert to ctypes arrays
        in_arr = (ctypes.c_double * len(flat_inputs))(*flat_inputs)
        tgt_arr = (ctypes.c_double * len(flat_targets))(*flat_targets)
        
        return self.lib.Helix_AI_Fit(
            in_arr, tgt_arr,
            num_samples, in_size, out_size,
            epochs, log_interval
        )
