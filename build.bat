@echo off
setlocal

echo Checking for compilers...

where cmake >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo CMake found. Building with CMake...
    if not exist build mkdir build
    cd build
    cmake ..
    if %ERRORLEVEL% EQU 0 (
        cmake --build .
        if %ERRORLEVEL% EQU 0 (
            echo Build Successful! Running tests...
            echo.
            echo [ALU Test]
            Debug\test_alu.exe || ./test_alu
            echo.
            echo [CPU Test]
            Debug\test_cpu.exe || ./test_cpu
            echo.
            echo [AI Test]
            Debug\test_ai.exe || ./test_ai
            echo.
            @REM echo [Graphics Test]
            @REM Debug\test_graphics.exe || ./test_graphics
            @REM echo.
            
            echo.
            echo [MLP Demo]
            Debug\demo_mlp.exe || ./demo_mlp
            echo.
            echo [Learning Demo]
            Debug\demo_learning.exe || ./demo_learning
            goto :end
        )
    )
    cd ..
)

    echo g++ found. Compiling directly...
    g++ src/trit_word.cpp src/test_alu.cpp -o test_alu.exe
    if %ERRORLEVEL% EQU 0 (
        echo [ALU] Build Successful!
    )
    
    g++ src/trit_word.cpp src/memory.cpp src/cpu.cpp src/test_cpu.cpp -o test_cpu.exe
    if %ERRORLEVEL% EQU 0 (
        echo [CPU] Build Successful!
        echo.
        echo Running ALU Test...
        test_alu.exe
        echo.
        echo Running CPU Test...
        test_cpu.exe
    )
    
    g++ src/trit_word.cpp src/soft_float.cpp src/test_ai.cpp -o test_ai.exe
    if %ERRORLEVEL% EQU 0 (
        echo [AI] Build Successful!
        echo.
        echo Running AI Test...
        test_ai.exe
    )

    g++ src/trit_word.cpp src/memory.cpp src/graphics.cpp src/test_graphics.cpp -o test_graphics.exe
    if %ERRORLEVEL% EQU 0 (
        echo [GPU] Build Successful!
        echo.
        echo Running Graphics Test...
        test_graphics.exe
    )
    
    g++ src/trit_word.cpp src/soft_float.cpp src/demo_mlp.cpp -o demo_mlp.exe
    if %ERRORLEVEL% EQU 0 (
        echo [MLP] Build Successful!
        echo.
        echo Running MLP Demo...
        demo_mlp.exe
    )

    g++ src/trit_word.cpp src/soft_float.cpp src/demo_learning.cpp -o demo_learning.exe
    if %ERRORLEVEL% EQU 0 (
        echo [Learning] Build Successful!
        echo.
        // We do NOT auto-run learning every time now, it takes too long.
        // echo Running Learning Demo...
        // demo_learning.exe
    )

    g++ src/trit_word.cpp src/soft_float.cpp src/run_model.cpp -o run_model.exe
    if %ERRORLEVEL% EQU 0 (
        echo [Inference] Build Successful!
        echo.
        echo Running Inference...
        run_model.exe
        goto :end
    )
)

echo.
echo [ERROR] No suitable compiler/build tool found (cmake or g++).
echo Please install GCC (MinGW) or CMake to run this PoC.

:end
pause
