import os

def inline_asm(asm_path, output_path):
    with open(asm_path, 'r') as f:
        lines = f.readlines()
        
    with open(output_path, 'w') as out:
        for line in lines:
            if line.strip().upper().startswith(".INCLUDE"):
                # Parse filename
                parts = line.strip().split()
                if len(parts) > 1:
                    include_file = parts[1].strip('"')
                    # Expect relative path
                    base_dir = os.path.dirname(asm_path)
                    full_path = os.path.join(base_dir, os.path.basename(include_file))
                    
                    if os.path.exists(full_path):
                        print(f"Inlining {full_path}...")
                        with open(full_path, 'r') as inc:
                            out.write(f"; --- Start of {include_file} ---\n")
                            out.write(inc.read())
                            out.write(f"\n; --- End of {include_file} ---\n")
                    else:
                        print(f"Error: Include file not found: {full_path}")
                        out.write(f"; ERROR: Could not find {include_file}\n")
            else:
                out.write(line)

inline_asm("experiments/nlp/rnn_inference.hasm", "experiments/nlp/rnn_inference_merged.hasm")
print("Done.")
