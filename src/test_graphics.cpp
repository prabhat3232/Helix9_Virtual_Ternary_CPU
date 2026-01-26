#include "graphics.h"
#include <iostream>

int main() {
    std::cout << "--- Helix-9 Graphics Test ---" << std::endl;
    
    TernaryMemory mem;
    int width = 120;
    int height = 120;
    int64_t vram = 1000; // VRAM at address 1000
    
    TernaryDisplay display(mem, width, height, vram);
    
    std::cout << "Generating Gradient Pattern in VRAM..." << std::endl;
    
    // Simulate GPU Kernel: Write pixels
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int64_t addr = vram + (y * width + x);
            
            // Create a pattern
            // Red increases with X
            // Green increases with Y
            // Blue oscillates
            
            // Map X (0..119) to Trit Range (-13..13)
            // -13 + (x / 120) * 26
            int64_t r_trit = -13 + (x * 26 / width);
            int64_t g_trit = -13 + (y * 26 / height);
            int64_t b_trit = (x % 26) - 13; // Sawtooth
            
            // Encode into one Int64 value
            // Val = R + G*27 + B*729
            // Note: This math works for positive integers. For balanced ternary,
            // we must be careful. 
            // A val of -13 in trits 0-2 is just -13.
            // A val of -13 in trits 3-5 is -13 * 27.
            // So Linear combination works!
            
            int64_t pixel_val = r_trit + (g_trit * 27) + (b_trit * 729);
            
            mem.Write(TernaryWord::FromInt64(addr), TernaryWord::FromInt64(pixel_val));
        }
    }
    
    std::cout << "Rendering to output.ppm..." << std::endl;
    display.SaveToPPM("output.ppm");
    
    return 0;
}
