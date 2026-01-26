#include "graphics.h"
#include <fstream>
#include <iostream>
#include <algorithm>

TernaryDisplay::TernaryDisplay(TernaryMemory& memory, int w, int h, int64_t vram_addr)
    : mem(memory), width(w), height(h), vram_base(vram_addr) {}

uint8_t TernaryDisplay::TritsToByte(int64_t val) {
    // 3 Trits in balanced ternary:
    // Max: 1*9 + 1*3 + 1 = 13
    // Min: -1*9 - 1*3 - 1 = -13
    // Range: 27 values (-13 to +13)
    
    // Map -13..13 to 0..26
    int64_t normalized = val + 13;
    
    // Map 0..26 to 0..255
    // 255 / 26 ~= 9.8
    int64_t rgb = normalized * 255 / 26;
    
    // Clamp just in case
    if (rgb < 0) rgb = 0;
    if (rgb > 255) rgb = 255;
    
    return (uint8_t)rgb;
}

void TernaryDisplay::SaveToPPM(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[Graphics] Error: Could not create " << filename << std::endl;
        return;
    }

    // PPM Header (P3 = Text RGB)
    file << "P3\n";
    file << width << " " << height << "\n";
    file << "255\n"; // Max Color

    // Pixel Data
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int64_t offset = y * width + x;
            TernaryWord pixel_data = mem.Read(TernaryWord::FromInt64(vram_base + offset));
            int64_t raw = pixel_data.ToInt64();
            
            // Extract Channels (3 trits each)
            // Low 9 trits used.
            // R: Trits 0-2 (Val % 27)
            // G: Trits 3-5 ((Val / 27) % 27)
            // B: Trits 6-8 ((Val / 729) % 27)
            
            // Note: Integer division on negative numbers in C++ rounds towards zero usually, 
            // but for bitwise-like extraction on "balanced ternary integer", standard div/mod 
            // works if we assume the integer IS the value.
            // However, we implemented `ToInt64` to convert balanced trits to int64.
            // So `1` in trits 3-5 is value 27.
            // We need to isolate the components.
            
            // A safer way without implementing `GetTrit(i)` everywhere:
            // Use bitwise logic via `ToInt64`?
            // Wait, we need the *value* of the sub-trits.
            
            // Re-implementing trit extraction logic here for PoC simplicity:
            // We need to decode the Int64 back to digits?
            // Actually, since `TernaryWord` wraps `pos` and `neg` bits, 
            // we can access bits directly if we made them public (they are).
            
            int64_t r_val = 0;
            int64_t g_val = 0;
            int64_t b_val = 0;
            
            // R (Trits 0,1,2)
            auto get_trits_val = [&](int start_bit) {
                int64_t v = 0;
                int64_t p3 = 1;
                for(int i=0; i<3; ++i) {
                    int bit = start_bit + i;
                    bool p = (pixel_data.pos >> bit) & 1;
                    bool n = (pixel_data.neg >> bit) & 1;
                    if (p) v += p3;
                    if (n) v -= p3;
                    p3 *= 3;
                }
                return v;
            };
            
            r_val = get_trits_val(0);
            g_val = get_trits_val(3);
            b_val = get_trits_val(6);
            
            file << (int)TritsToByte(r_val) << " " 
                 << (int)TritsToByte(g_val) << " " 
                 << (int)TritsToByte(b_val) << " ";
        }
        file << "\n";
    }
    
    file.close();
    std::cout << "[Graphics] Saved frame to " << filename << std::endl;
}
