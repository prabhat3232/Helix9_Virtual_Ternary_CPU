int result_fact;
int result_math;
int result_logic;

int factorial(int n) {
    int res = 1;
    while (n > 1) {
        res = res * n;
        n = n - 1;
    }
    return res;
}

int math_test(int a, int b, int c, int d) {
    // (10 + 2) * (5 - 3) = 12 * 2 = 24
    return (a + b) * (c - d);
}

int logic_test(int a, int b) {
    if (a > b) {
        if (b > 0) {
            return 1;
        }
    }
    return 0;
}

int main() {
    // Test 1: Factorial(5) = 120
    result_fact = factorial(5);
    
    // Test 2: Math (10+2)*(5-3) = 24
    result_math = math_test(10, 2, 5, 3);
    
    // Test 3: Logic (5 > 2) && (2 > 0) -> 1
    result_logic = logic_test(5, 2);
    
    // Verify Results
    if (result_fact != 120) {
        return 1; // Error: Factorial failed
    }
    if (result_math != 24) {
        return 2; // Error: Math failed
    }
    if (result_logic != 1) {
        return 3; // Error: Logic failed
    }
    
    // Success Code
    return 111;
}
