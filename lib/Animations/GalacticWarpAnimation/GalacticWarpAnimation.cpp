#include "GalacticWarpAnimation.hpp"
#include <FastLED.h>

GalacticWarpAnimation::GalacticWarpAnimation(uint16_t id, LedMatrix& m)
	: AnimationBase(GALACTICWARP_DEFAULT_HUE, id, m) {}

void GalacticWarpAnimation::render() {
	if (!isInitialized()) return;
	matrix.clear();

	int w = mw;
	int hgt = mh;

	uint16_t t = (uint16_t)(millis() / 5);
	uint8_t centerX = (uint8_t)(w / 2);

	uint8_t baseHue = animCfg.hue;
	for (int x = 0; x < w; ++x) {
		int dist = abs(x - (int)centerX);
		uint8_t wave1 = sin8((uint8_t)(t + dist * 8));
		uint8_t wave2 = sin8((uint8_t)(t - dist * 6 + 85));
		uint8_t wave3 = sin8((uint8_t)(t + dist * 4 + 170));

		uint8_t intensity = qadd8(wave1, scale8(qadd8(wave2, wave3), 128));

		for (int y = 0; y < hgt; ++y) {
			uint8_t spiralHue = (uint8_t)(baseHue + t / 2 + dist * 4 + y * 64);
			uint8_t vOut = scale8(GALACTICWARP_DEFAULT_VAL, intensity);
			matrix.setPixelHSV(x, y, spiralHue, GALACTICWARP_DEFAULT_SAT, vOut);
		}
	}

	int coreX = (int)centerX;
	for (int y = 0; y < hgt; ++y) {
		uint8_t coreBrightness = 255 - (uint8_t)((millis() / 3) % 256);
		uint8_t hOut = (uint8_t)(baseHue + (t & 0xFF) / 2);
		matrix.setPixelHSV(coreX, y, hOut, GALACTICWARP_DEFAULT_SAT, coreBrightness);
	}

}
