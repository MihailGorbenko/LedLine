#include "PowerOnAnimation.hpp"

void PowerOnAnimation::render() {
    matrix.clear();

    int w = matrix.width();
    int h = matrix.height();

    // Противоположное направление к PowerOff:
    // progress=255 => все столбцы полные; растет справа-налево
    int filledFromRight = (int)((uint32_t)progress * (uint32_t)w / 255U);
    int rem = (int)(((uint32_t)progress * (uint32_t)w) % 255U); // 0..254
    int startFull = w - filledFromRight; // индексация полной области [startFull..w-1]

    for (int x = 0; x < w; ++x) {
        uint8_t colV = 0;
        if (x > startFull && filledFromRight > 0) {
            // внутри полной правой области
            colV = val;
        } else if (x == startFull && filledFromRight > 0) {
            // частичная пограничная колонка
            colV = (uint8_t)((uint32_t)rem * (uint32_t)val / 255U);
        } else {
            colV = 0;
        }
        for (int y = 0; y < h; ++y) {
            if (colV > 0) matrix.setPixelHSV(x, y, hue, sat, colV);
        }
    }

}
