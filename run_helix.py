
import os
import sys
import subprocess
import argparse

# Configuration
# Configuration
COMPILER = os.path.normpath("src/compiler/py/helix_py_compiler.py")
ASSEMBLER = os.path.normpath("build/bin/Debug/helix_asm.exe")
EMULATOR = os.path.normpath("build/bin/Debug/helix_emu.exe")
PYTHON = sys.executable

def run_command(cmd, cwd=None, timeout=10):
    print(f"[RUN] {cmd}")
    try:
        result = subprocess.run(cmd, shell=True, check=True, cwd=cwd, capture_output=True, text=True, timeout=timeout)
        return result.stdout
    except subprocess.TimeoutExpired as e:
        print(f"Error: Command '{cmd}' timed out after {timeout} seconds.")
        if e.stdout: print(f"--- Stdout ---\n{e.stdout}")
        if e.stderr: print(f"--- Stderr ---\n{e.stderr}")
        sys.exit(1)
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {cmd}")
        print(e.stderr)
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser(description="Run Helix Python Script")
    parser.add_argument("input", help="Input Python file")
    parser.add_argument("--cycles", type=int, default=500000, help="Max Emulation Cycles")
    args = parser.parse_args()
    
    input_file = args.input
    base_name = os.path.splitext(input_file)[0]
    hasm_file = base_name + ".hasm"
    ht_file = base_name + ".ht"
    
    # 1. Compile
    print(f"--- 1. Compiling {input_file} ---")
    cmd_compile = f'"{PYTHON}" {COMPILER} {input_file}'
    stdout = run_command(cmd_compile)
    
    with open(hasm_file, "w") as f:
        f.write(stdout)
    print(f"Generated {hasm_file}")
    
    # 2. Assemble
    print(f"--- 2. Assembling {hasm_file} ---")
    cmd_assemble = f"{ASSEMBLER} {hasm_file} -o {ht_file}"
    run_command(cmd_assemble)
    print(f"Generated {ht_file}")
    
    # 3. Emulate
    print(f"--- 3. Emulating {ht_file} ---")
    cmd_emulate = f"{EMULATOR} {ht_file} {args.cycles}"
    # Run directly to see output
    os.system(cmd_emulate)

if __name__ == "__main__":
    main()
