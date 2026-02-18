
# Helix Python Compiler - Matrix Feature Demo
try:
    from helix_std import Matrix
except ImportError:
    pass # Compiler doesn't resolve imports, but that's fine

# 1. Matrix Declaration (Fixed Size) with Initialization for Python Execution
# The compiler uses the Annotation to allocate .DAT.
# The Python runtime uses the Assignment to initialize the object.
A: Matrix[2, 2] = Matrix(2, 2, fill=2)
B: Matrix[2, 2] = Matrix(2, 2, fill=3)
C: Matrix[2, 2] = Matrix(2, 2, fill=0)
D: Matrix[2, 2] = Matrix(2, 2, fill=0)

# 2. Main Logic
def main():
    print("--- Helix Matrix Demo ---")
    print("A (Fill 2):")
    print(A)
    print("B (Fill 3):")
    print(B)

    # Matrix Multiplication
    # C = A @ B  (Uses scalar nested loops in ASM, __matmul__ in Python)
    # [2 2] @ [3 3] = [12 12]
    # [2 2]   [3 3]   [12 12]
    global C
    C = A @ B
    print("C = A @ B (Expected 12s):")
    print(C)
    
    # Element-wise Operations
    # D = A + B -> [5 5]
    global D
    D = A + B
    print("D = A + B (Expected 5s):")
    print(D)
    
    # C = C - D -> [12-5=7]
    C = C - D
    print("C = C - D (Expected 7s):")
    print(C)
    
    # Example Scalar Logic Integration
    i = 0
    print("Running Loop...")
    while i < 10:
        i = i + 1
        if i > 5:
            # Perform another matrix op inside control flow
            D = D + C
            
    print("Final D after loop (D + 5*C = 5 + 5*7 = 40):")
    print(D)

if __name__ == "__main__":
    main()
