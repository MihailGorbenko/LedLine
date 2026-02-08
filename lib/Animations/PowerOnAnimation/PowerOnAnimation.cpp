#include "PowerOnAnimation.hpp"

void PowerOnAnimation::render() {
    matrix.clear();

    int w = matrix.width();
    int h = matrix.height();

    // Fill left-to-right: progress=255 => all columns full; grows from left to right
    int filledColumns = (int)((uint32_t)progress * (uint32_t)w / 255U);
    int rem = (int)(((uint32_t)progress * (uint32_t)w) % 255U); // 0..254

    for (int x = 0; x < w; ++x) {
        uint8_t colV = 0;
        if (x < filledColumns && filledColumns > 0) {
            // fully inside filled left area
            colV = val;
        } else if (x == filledColumns && filledColumns < w) {
            // partial boundary column
            colV = (uint8_t)((uint32_t)rem * (uint32_t)val / 255U);
        } else {
            colV = 0;
        }
        for (int y = 0; y < h; ++y) {
            if (colV > 0) matrix.setPixelHSV(x, y, hue, sat, colV);
        }
    }

}
