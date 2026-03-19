// main.cpp
#include "capture.h"
#include <cstdio>

int main() {
    Screenshot s = takeScreenshot();

    if (!s.pixels) {
        printf("Fehler: kein Screenshot\n");
        return 1;
    }

    printf("Screenshot: %d x %d px\n", s.width, s.height);

    // Pixel auslesen — reines C++
    int x = 100, y = 100;
    uint8_t* px = s.pixels + y * s.bytesPerRow + x * 4;
    printf("Pixel (%d,%d): R=%d G=%d B=%d\n", x, y, px[2], px[1], px[0]);

    freeScreenshot(s);
    return 0;
}
