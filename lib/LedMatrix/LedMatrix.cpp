#include "LedMatrix.hpp"


LedMatrix::LedMatrix() {
}


void LedMatrix::init() {
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 3000); // Ограничение по питанию
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(255); // используем 255 — яркость управляется вручную через masterBrightness и baseLeds
    clear();
    update();
}

void LedMatrix::clear() {
    fill_solid(baseLeds, NUM_LEDS, CRGB::Black); // ...очищаем буфер оригинальных цветов...
    fill_solid(leds, NUM_LEDS, CRGB::Black);
}

void LedMatrix::update() {
    FastLED.show();
}


void LedMatrix::setPixelHSV(int x, int y, uint8_t h, uint8_t s, uint8_t v) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return;
    }
    int index = XY(x, y);
    if (index >= 0) {
        // Сохраняем исходный цвет в RGB, затем масштабируем копию для вывода
        CRGB orig = CHSV(h, s, v);
        baseLeds[index] = orig;
        CRGB out = orig;
        out.nscale8_video(masterBrightness); // сохраняет относительные пропорции каналов → насыщенность не «выгорает»
        leds[index] = out;
    }
}

void LedMatrix::powerOff() {
    clear();
    update();
}


void LedMatrix::setMasterBrightness(uint8_t b) {
    // Привести вход к диапазону 0..255
    uint8_t newB = (uint8_t)constrain(b, 0, 255);

    if (newB == this->masterBrightness) {
        return;
    }

    this->masterBrightness = newB;
    // Пересчитать все выводимые цвета из baseLeds с новым масштабом
    for (int i = 0; i < NUM_LEDS; ++i) {
        CRGB out = baseLeds[i];
        out.nscale8_video(this->masterBrightness);
        leds[i] = out;
    }
    update(); // показываем текущее состояние с новой яркостью
}

int LedMatrix::XY(int x, int y) {
    // Преобразование координат (x, y) в индекс массива
    // Серпантиновый маппинг: чётные строки слева->справа, нечётные справа->слева
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return -1; // безопасный возврат при некорректных координатах
    }
    if (y % 2 == 0) { // 0,2,4... — слева направо
        return y * width + x;
    } else { // 1,3,5... — справа налево
        return y * width + (width - 1 - x);
    }
}

