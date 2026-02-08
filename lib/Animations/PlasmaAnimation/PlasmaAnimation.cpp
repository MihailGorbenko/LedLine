#include "PlasmaAnimation.hpp"
#include <FastLED.h>

PlasmaAnimation::PlasmaAnimation(uint16_t id, LedMatrix& m)
	: AnimationBase(PLASMA_DEFAULT_HUE, id, m) {}

void PlasmaAnimation::render() {
	if (!isInitialized()) return;
	matrix.clear();

	int w = mw;
	int hgt = mh;

	uint16_t t = (uint16_t)(millis() / 4);

	uint8_t baseHue = getConfig().hue;
	for (int x = 0; x < w; ++x) {
		uint8_t sx = sin8((uint8_t)(x * 8 + (t & 0xFF)));
		for (int y = 0; y < hgt; ++y) {
			uint8_t sy = sin8((uint8_t)(y * 48 + ((t >> 1) & 0xFF)));
			uint8_t p = (uint8_t)((sx + sy) >> 1);

			// Hue slowly drifts, value is modulated by plasma field
			uint8_t hOut = (uint8_t)(baseHue + (t >> 3) + (p >> 2));
			// allow full brightness at peaks; keep small floor of 16
			uint8_t vOut = scale8(PLASMA_DEFAULT_VAL, qadd8(16, p));
			matrix.setPixelHSV(x, y, hOut, PLASMA_DEFAULT_SAT, vOut);
		}
	}
}
