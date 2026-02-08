#include "SparkleWaveAnimation.hpp"
#include <FastLED.h>

SparkleWaveAnimation::SparkleWaveAnimation(uint16_t id, LedMatrix& m)
	: AnimationBase(SPARKLEWAVE_DEFAULT_HUE, id, m),
	  sparkleChance(28) {}

void SparkleWaveAnimation::render() {
	if (!isInitialized()) return;
	matrix.clear();

	int w = mw;
	int hgt = mh;

	uint16_t t = (uint16_t)(millis() / 5);
	uint8_t baseHue = animCfg.hue;
	for (int x = 0; x < w; ++x) {
		uint8_t wave = sin8((uint8_t)(t + x * 10));
		uint8_t vWave = scale8(SPARKLEWAVE_DEFAULT_VAL, qadd8(0, wave));
		uint8_t hOut = (uint8_t)(baseHue + (wave >> 2));

		for (int y = 0; y < hgt; ++y) {
			uint8_t vOut = (hgt >= 2) ? ((y == 0) ? vWave : scale8(vWave, 200)) : vWave;
			if ((uint8_t)random(0, 255) < sparkleChance) {
				vOut = qadd8(vOut, 120);
			}
			matrix.setPixelHSV(x, y, hOut, SPARKLEWAVE_DEFAULT_SAT, vOut);
		}
	}
}
