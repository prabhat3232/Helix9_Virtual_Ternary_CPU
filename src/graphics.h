#pragma once
#include "memory.h"
#include <string>
#include <vector>

// Graphics Specs
// 9 Trits per Pixel (R:3, G:3, B:3)
class TernaryDisplay {
public:
    int width;
    int height;
    TernaryMemory& mem;
    int64_t vram_base; // Start address of VRAM

public:
    TernaryDisplay(TernaryMemory& memory, int w, int h, int64_t vram_addr);

    // Dumps the current VRAM state to a PPM image file
    void SaveToPPM(const std::string& filename);

private:
    // Helper to Convert 3 Trits (-13..13) to 8-bit Color (0..255)
    uint8_t TritsToByte(int64_t val);
};
