#include "RainbowChaseAnimation.hpp"
#include <FastLED.h>

RainbowChaseAnimation::RainbowChaseAnimation(uint16_t id, LedMatrix& m)
	: AnimationBase(RAINBOWCHASE_DEFAULT_HUE, id, m) {}

void RainbowChaseAnimation::render() {
	if (!isInitialized()) return;
	matrix.clear();

	int w = mw;
	int hgt = mh;

	const uint8_t speed = 6; // higher = faster
	uint8_t t = (uint8_t)((millis() / speed) & 0xFF);

	uint8_t baseHue = getConfig().hue;
	// Moving rainbow, slight row offset for 2-row matrices
	for (int x = 0; x < w; ++x) {
		uint8_t xHue = (uint8_t)(baseHue + t + (uint8_t)((x * 256) / max(1, w)));
		for (int y = 0; y < hgt; ++y) {
			uint8_t rowShift = (uint8_t)(y * 24);
			matrix.setPixelHSV(x, y, (uint8_t)(xHue + rowShift), RAINBOWCHASE_DEFAULT_SAT, RAINBOWCHASE_DEFAULT_VAL);
		}
	}
}
