// ocr.h
#pragma once
#include <string>

struct CropRegion {
    int x, y, width, height;
};

// Liest Text aus einem Bildausschnitt
std::string readText(const uint8_t* pixels, int imgWidth, int imgHeight,
                     int bytesPerRow, CropRegion region);
