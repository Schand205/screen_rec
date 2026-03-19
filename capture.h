// capture.h
#pragma once
#include <cstdint>

struct Screenshot {
    uint8_t* pixels;   // BGRA Rohdaten
    int      width;
    int      height;
    int      bytesPerRow;
};

// Einzige Funktion die C++ aufruft
Screenshot takeScreenshot();
void       freeScreenshot(Screenshot& s);
