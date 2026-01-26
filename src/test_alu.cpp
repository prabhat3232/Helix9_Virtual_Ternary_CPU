#include <iostream>
#include <cassert>
#include "trit_word.h"

void TestConversion() {
    std::cout << "Testing Conversion..." << std::endl;
    for (int64_t i = -100; i <= 100; ++i) {
        TernaryWord t = TernaryWord::FromInt64(i);
        int64_t back = t.ToInt64();
        if (i != back) {
            std::cout << "FAIL: " << i << " -> " << t.ToString() << " -> " << back << std::endl;
            exit(1);
        }
    }
    std::cout << "Conversion Passed." << std::endl;
}

void TestLogic() {
    std::cout << "Testing Logic..." << std::endl;
    // +1 (p=1, n=0)
    // -1 (p=0, n=1)
    TernaryWord one = TernaryWord::FromInt64(1);
    TernaryWord neg_one = TernaryWord::FromInt64(-1);
    TernaryWord zero = TernaryWord::FromInt64(0);

    // Min (AND)
    // (+1, -1) -> -1
    TernaryWord min_bad = one.Min(neg_one);
    assert(min_bad.ToInt64() == -1);

    // Max (OR)
    // (+1, -1) -> +1
    TernaryWord max_good = one.Max(neg_one);
    assert(max_good.ToInt64() == 1);
    
    std::cout << "Logic Passed." << std::endl;
}

void TestArithmetic() {
    std::cout << "Testing Arithmetic..." << std::endl;
    
    // 1 + 1 = 2
    assert(TernaryWord::FromInt64(1).Add(TernaryWord::FromInt64(1)).ToInt64() == 2);
    
    // 1 + (-1) = 0
    assert(TernaryWord::FromInt64(1).Add(TernaryWord::FromInt64(-1)).ToInt64() == 0);
    
    // 10 + 5 = 15
    assert(TernaryWord::FromInt64(10).Add(TernaryWord::FromInt64(5)).ToInt64() == 15);
    
    // Large Numbers
    // 1000 - 1 = 999
    assert(TernaryWord::FromInt64(1000).Add(TernaryWord::FromInt64(-1)).ToInt64() == 999);
    
    std::cout << "Arithmetic Passed." << std::endl;
}

int main() {
    TestConversion();
    TestLogic();
    TestArithmetic();
    
    std::cout << "All Tests Passed!" << std::endl;
    return 0;
}
