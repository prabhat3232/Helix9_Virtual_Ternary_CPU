def rec(n):
    if n == 0:
        return 0
    return 1 + rec(n-1)

def main():
    return rec(5)
