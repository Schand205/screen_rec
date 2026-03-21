// main.cpp
#include "capture.h"
#include "ocr.h"
#include <cstdio>
#include <CoreGraphics/CoreGraphics.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main() {
    // 1. Screenshot machen
    Screenshot s = takeScreenshot();
    if (!s.pixels) {
        printf("Screenshot fehlgeschlagen\n");
        return 1;
    }

    // 2. Retina-Skalierung berechnen
    CGDirectDisplayID displayID = CGMainDisplayID();
    size_t logicalW = CGDisplayPixelsWide(displayID);
    size_t logicalH = CGDisplayPixelsHigh(displayID);
    float  scale    = (float)s.width / (float)logicalW;

    printf("Raw:     %d x %d px\n", s.width, s.height);
    printf("Logical: %zu x %zu px\n", logicalW, logicalH);
    printf("Scale:   %.1fx\n", scale);

    int outW = (int)logicalW;
    int outH = (int)logicalH;

    // 3. BGRA → RGBA + Retina-Downscale
    uint8_t* rgba = new uint8_t[outW * outH * 4];

    for (int y = 0; y < outH; y++) {
        for (int x = 0; x < outW; x++) {
            int srcX = (int)(x * scale);
            int srcY = (int)(y * scale);
            const uint8_t* src = s.pixels + srcY * s.bytesPerRow + srcX * 4;
            uint8_t* dst = rgba + (y * outW + x) * 4;
            dst[0] = src[2]; // R  (BGRA → RGBA)
            dst[1] = src[1]; // G
            dst[2] = src[0]; // B
            dst[3] = src[3]; // A
        }
    }

    // 4. PNG speichern (zum Debuggen / Region finden)
    stbi_write_png("screenshot.png", outW, outH, 4, rgba, outW * 4);
    printf("Gespeichert: screenshot.png\n");

    // 5. OCR auf Pokémon-Name Region
    //    → erst screenshot.png in Preview öffnen und Region bestimmen
    //    → dann x, y, width, height hier anpassen
    CropRegion nameRegion = {
        10,  // x
        135,  // y
	645,  // width
        25    // height
    };

    // OCR arbeitet auf den skalierten RGBA-Daten
    // Wir bauen dafür einen temporären Screenshot-Wrapper
    Screenshot scaled;
    scaled.pixels     = rgba;
    scaled.width      = outW;
    scaled.height     = outH;
    scaled.bytesPerRow = outW * 4;

    std::string name = readText(scaled.pixels, scaled.width, scaled.height,
                                scaled.bytesPerRow, nameRegion);

    printf("Erkannter Name: '%s'\n", name.c_str());

    // 6. Aufräumen
    delete[] rgba;
    freeScreenshot(s);
    return 0;
}
