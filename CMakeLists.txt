cmake_minimum_required(VERSION 3.10)
project(Helix9)

set(CMAKE_CXX_STANDARD 17)

# Core Library
add_library(helix9_core
    src/trit_word.cpp
    src/trit_word.h
    src/memory.cpp
    src/memory.h
    src/cpu.cpp
    src/cpu.h
    src/soft_float.cpp
    src/soft_float.h
    src/soft_float.h
    src/vector_alu.h
    src/graphics.cpp
    src/graphics.h
)

# Python Bindings (DLL)
add_library(helix9_lib SHARED
    src/bindings.cpp
)
target_link_libraries(helix9_lib helix9_core)

# Test Executables
add_executable(test_alu src/test_alu.cpp)
target_link_libraries(test_alu helix9_core)

add_executable(test_cpu src/test_cpu.cpp)
target_link_libraries(test_cpu helix9_core)

add_executable(test_ai src/test_ai.cpp)
target_link_libraries(test_ai helix9_core)

add_executable(test_graphics src/test_graphics.cpp)
target_link_libraries(test_graphics helix9_core)

add_executable(demo_mlp src/demo_mlp.cpp)
target_link_libraries(demo_mlp helix9_core)

add_executable(demo_learning src/demo_learning.cpp)
target_link_libraries(demo_learning helix9_core)

add_executable(run_model src/run_model.cpp)
target_link_libraries(run_model helix9_core)

add_executable(demo_generative src/demo_generative.cpp)
target_link_libraries(demo_generative helix9_core)
