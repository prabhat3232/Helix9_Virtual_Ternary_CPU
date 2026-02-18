
# Test Matrix Element Access
# Expected Output:
# 50
# 80
# 99

A: Matrix[2, 2] = [(10, 20),
                   (30, 40)]

# Test 1: Read Element
x = A[1, 1] # 40
y = A[0, 0] # 10
z = x + y   # 50
print(z)

# Test 2: Write Element
A[0, 1] = 80
val = A[0, 1]
print(val) # 80

# Test 3: Write with calculation
A[1, 0] = x + y + 49 # 50 + 49 = 99
print(A[1, 0])
