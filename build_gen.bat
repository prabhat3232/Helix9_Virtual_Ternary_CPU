@echo off
echo [Generative AI] Building with CMake...
cmake --build build --target demo_generative --config Debug
if %ERRORLEVEL% EQU 0 (
    echo Build Successful!
    echo.
    echo Running Generative AI...
    build\Debug\demo_generative.exe
) else (
    echo Build Failed!
)
pause
