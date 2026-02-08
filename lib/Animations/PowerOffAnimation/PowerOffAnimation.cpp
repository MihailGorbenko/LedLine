#include "PowerOffAnimation.hpp"

void PowerOffAnimation::render() {
    matrix.clear();

    int w = matrix.width();
    int h = matrix.height();

    // заполнение по ширине слева-направо: progress=255 => все колонки полные
    int filledColumns = (int)((uint32_t)progress * (uint32_t)w / 255U);
    int rem = (int)(((uint32_t)progress * (uint32_t)w) % 255U); // 0..254
    for (int x = 0; x < w; ++x) {
        uint8_t colV = 0;
        if (x < filledColumns) {
            colV = val;
        } else if (x == filledColumns) {
            colV = (uint8_t)((uint32_t)rem * (uint32_t)val / 255U);
        } else {
            colV = 0;
        }
        for (int y = 0; y < h; ++y) {
            if (colV > 0) matrix.setPixelHSV(x, y, hue, sat, colV);
        }
    }

}

