#ifndef LED_MATRIX_HPP
#define LED_MATRIX_HPP
#include <FastLED.h>

#define MATRIX_WIDTH 47
#define MATRIX_HEIGHT 2
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)
#define DEF_BRIGHTNESS 128
#define LED_PIN 2

/// Класс управления светодиодной матрицей
class LedMatrix {
private:
    CRGB leds[NUM_LEDS];
    CRGB baseLeds[NUM_LEDS]; // буфер оригинальных (незашкалированных) цветов
    int width = MATRIX_WIDTH;
    int height = MATRIX_HEIGHT;
    uint8_t masterBrightness = DEF_BRIGHTNESS; // Мастер яркость (глобальная)
public:

    LedMatrix();
    void init();
    CRGB* getLeds();
    void clear();
    void update();
    void powerOff();
    int XY(int x, int y);
    int getWidth() const{ return width; };
    int getHeight() const{ return height; };
    int getNumLeds() const{ return NUM_LEDS; };
    void setMasterBrightness(uint8_t b);
    void setPixelHSV(int x, int y, uint8_t h, uint8_t s, uint8_t v);
};

#endif // LED_MATRIX_HPP