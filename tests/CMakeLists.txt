cmake_minimum_required(VERSION 3.10)
project(HelixCognitiveTest)

set(CMAKE_CXX_STANDARD 20)

include_directories(../src)

add_executable(test_cognitive 
    test_cognitive.cpp 
    ../src/trit_word.cpp
)
