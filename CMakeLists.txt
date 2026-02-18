cmake_minimum_required(VERSION 3.10)
project(Helix9)

set(CMAKE_CXX_STANDARD 17)

# Output Directory (Simplify CTest discovery)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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
    src/cognitive_trace.cpp
    src/cognitive_trace.h
    src/cognitive/scheduler.cpp
    src/cognitive/stability_monitor.cpp
    src/cognitive/reward_engine.cpp
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

# --- Assembler ---
add_executable(helix_asm 
    src/assembler/main.cpp 
    src/assembler/Lexer.cpp 
    src/assembler/Parser.cpp
    src/trit_word.cpp 
)
target_link_libraries(helix_asm helix9_core)

# --- Compiler (Helix C-lite) ---
add_executable(helix_c
    src/compiler/main.cpp
    src/compiler/Lexer.cpp
    src/compiler/Parser.cpp
    src/compiler/CodeGen.cpp
)
target_link_libraries(helix_c helix9_core)

# --- Assembler Execution Test ---
add_executable(test_asm_exec src/test_asm_exec.cpp)
target_link_libraries(test_asm_exec helix9_core)

add_executable(test_asm_suite src/test_asm_suite.cpp)
target_link_libraries(test_asm_suite helix9_core)

add_executable(test_hash_suite src/test_hash_suite.cpp)
target_link_libraries(test_hash_suite helix9_core)

# --- Linker ---
add_executable(helix_ld
    src/linker/main.cpp
    src/linker/Linker.cpp
)
target_link_libraries(helix_ld helix9_core)

# --- Emulator ---
add_executable(helix_emu src/emulator.cpp)
target_link_libraries(helix_emu helix9_core)

# Experiment: Seeker Env
add_executable(seeker_env experiments/seeker_env.cpp)
target_link_libraries(seeker_env helix9_core)

# Cognitive Trace Test
add_executable(test_trace src/test_trace.cpp)
target_link_libraries(test_trace helix9_core)

# Advanced Memory Test
add_executable(test_advanced_memory src/test_advanced_memory.cpp)
target_link_libraries(test_advanced_memory helix9_core)

# Test: Cognitive Runtime
add_executable(test_cognitive_runtime src/tests/test_cognitive_runtime.cpp)
target_link_libraries(test_cognitive_runtime helix9_core)

# Experiment: 2D Seeker
add_executable(seeker_2d 
    experiments/s2d/s2d_main.cpp
    experiments/s2d/s2d_env.cpp
    experiments/s2d/s2d_agent.cpp
)
target_link_libraries(seeker_2d helix9_core)
# Experiment: Multi-Agent Swarm
add_executable(swarm_exp
    experiments/swarm/swarm_main.cpp
    experiments/swarm/swarm_env.cpp
    experiments/swarm/swarm_agent.cpp
)
target_link_libraries(swarm_exp helix9_core)
# Experiment: Resonance (Pattern Locking)
add_executable(resonance_exp
    experiments/resonance/resonance_main.cpp
    experiments/resonance/resonance_env.cpp
    experiments/resonance/resonance_agent.cpp
)
target_link_libraries(resonance_exp helix9_core)
# Benchmarks
add_executable(helix_bench
    benchmarks/benchmark_main.cpp
)
target_link_libraries(helix_bench helix9_core)
# Test: Multi-Agent Concurrency
add_executable(test_multi_agent 
    tests/test_multi_agent.cpp
    src/assembler/Parser.cpp
    src/assembler/Lexer.cpp
)
target_link_libraries(test_multi_agent helix9_core)

# --- CTest Configuration ---
enable_testing()

add_test(NAME TestALU COMMAND $<TARGET_FILE:test_alu>)
add_test(NAME TestCPU COMMAND $<TARGET_FILE:test_cpu>)
add_test(NAME TestAI COMMAND $<TARGET_FILE:test_ai>)
add_test(NAME TestMemory COMMAND $<TARGET_FILE:test_advanced_memory>)
add_test(NAME TestASMSuite COMMAND $<TARGET_FILE:test_asm_suite> test_suite.ht)
add_test(NAME TestCognitiveRuntime COMMAND $<TARGET_FILE:test_cognitive_runtime>)
add_test(NAME TestTrace COMMAND $<TARGET_FILE:test_trace>)
