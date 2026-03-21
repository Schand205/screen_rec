// ocr.cpp
#include "ocr.h"
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <cstring>

std::string readText(const uint8_t* pixels, int imgWidth, int imgHeight,
                     int bytesPerRow, CropRegion r) {

    // Grenzen prüfen
    if (r.x < 0 || r.y < 0 ||
        r.x + r.width  > imgWidth ||
        r.y + r.height > imgHeight) {
        return "CROP_OUT_OF_BOUNDS";
    }

    // 1. Crop als eigener RGBA-Buffer
    int w = r.width, h = r.height;
    uint8_t* crop = new uint8_t[w * h * 4];
    for (int row = 0; row < h; row++) {
        memcpy(crop + row * w * 4,
               pixels + (r.y + row) * bytesPerRow + r.x * 4,
               w * 4);
    }

    // 2. PIX über Leptonica erzeugen (verwaltet eigenen Speicher)
    PIX* pix = pixCreate(w, h, 32);
    l_uint32* data = pixGetData(pix);
    int wpl = pixGetWpl(pix); // "words per line"

    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            const uint8_t* px = crop + (row * w + col) * 4;
            // RGBA → Leptonica RGBA (big-endian packed)
            uint8_t r_ = px[0], g = px[1], b = px[2], a = px[3];
            data[row * wpl + col] =
                ((uint32_t)r_ << 24) |
                ((uint32_t)g  << 16) |
                ((uint32_t)b  <<  8) |
                ((uint32_t)a);
        }
    }
    delete[] crop;

    // 3. Tesseract
    tesseract::TessBaseAPI tess;
    tess.Init(nullptr, "eng");
    tess.SetPageSegMode(tesseract::PSM_SINGLE_LINE); // eine Zeile = Pokémon-Name
    tess.SetVariable("tessedit_char_whitelist",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz -");

    tess.SetImage(pix);
    char* raw = tess.GetUTF8Text();
    std::string result(raw ? raw : "");
    delete[] raw;
    tess.End();
    pixDestroy(&pix);

    // trailing whitespace/newlines entfernen
    while (!result.empty() &&
           (result.back() == '\n' || result.back() == ' ' || result.back() == '\r'))
        result.pop_back();

    return result;
}
