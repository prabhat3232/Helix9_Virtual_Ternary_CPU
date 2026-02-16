int limit = 2;
int result;

int fib(int n) {
    if (n <= 1) {
        return n;
    }
    return fib(n - 1) + fib(n - 2);
}

int main() {
    result = fib(limit);
    return result;
}
