
def main():
    # Matrix A [2x2]
    A: Matrix[2,2] = [
        (1, 2),
        (3, 4)
    ]
    
    print("Matrix A:")
    print(A)

    # Matrix B [2x3] filled with 9
    B: Matrix[2,3] = fill(9)
    print("Matrix B:")
    print(B)

    # Matrix C = A + A
    # C: Matrix[2,2]
    # C = A + A
    # print("Matrix C (A+A):")
    # print(C)
    # Compiler inferred type/addr for C might not work for 'print(C)' if C is local?
    # Actually, matrix vars are global in current scope impl? 
    # Let's stick to declared global matrices first as per current compiler limits.
