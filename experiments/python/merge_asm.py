import os

def merge_files(weight_file, inference_file, output_file):
    print(f"Merging {weight_file} and {inference_file} -> {output_file}")
    
    with open(output_file, 'w') as out:
        # 1. Write Inference Code (Skipping #include)
        with open(inference_file, 'r') as f:
            for line in f:
                if line.strip().startswith("#include"):
                    out.write(f"; {line.strip()} (Skipped by merge_asm.py - Weights appended at end)\n")
                else:
                    out.write(line)
                    if not line.endswith('\n'):
                        out.write('\n')
        
        out.write("\n; --- Weights (Appended) ---\n")
        out.write(".ORG 0x1000\n\n")
        
        # 2. Write Weights
        with open(weight_file, 'r') as f:
            for line in f:
                out.write(line)
                if not line.endswith('\n'):
                    out.write('\n')

if __name__ == "__main__":
    base_dir = "experiments/tnn"
    merge_files(
        os.path.join(base_dir, "mnist_weights.hasm"),
        os.path.join(base_dir, "mnist_inference.hasm"),
        os.path.join(base_dir, "mnist_full.hasm")
    )
