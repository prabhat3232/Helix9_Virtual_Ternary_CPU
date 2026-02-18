
# Factorial Test for Helix Python Compiler

def factorial(n):
    if n < 1:
        return 1
    else:
        return n * factorial(n - 1)

def main():
    return factorial(5) # Expect 120
