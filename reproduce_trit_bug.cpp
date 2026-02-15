#include <iostream>
#include "src/trit_word.h"

int main() {
    int64_t val = 999;
    TernaryWord tw = TernaryWord::FromInt64(val);
    int64_t out = tw.ToInt64();
    
    std::cout << "Input: " << val << " Output: " << out << std::endl;
    std::cout << "Pos: " << tw.pos << " Neg: " << tw.neg << std::endl;
    
    return (val == out) ? 0 : 1;
}
