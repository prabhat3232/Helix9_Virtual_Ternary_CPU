def test_cmp(a, b):
    if a < b:
        return 1
    else:
        return 0

def main():
    return test_cmp(10, 20) # Expect 1
