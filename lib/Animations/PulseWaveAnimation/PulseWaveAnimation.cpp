#include "PulseWaveAnimation.hpp"
#include <FastLED.h>

PulseWaveAnimation::PulseWaveAnimation(uint16_t id, LedMatrix& m)
	: AnimationBase(PULSEWAVE_DEFAULT_HUE, id, m),
	  pulseRadius(0) {}

void PulseWaveAnimation::render() {
	if (!isInitialized()) return;
	matrix.clear();

	int w = mw;
	int hgt = mh;

	// Cosmic pulse expanding from center
	uint16_t t = (uint16_t)(millis() / 6);
	uint8_t centerX = (uint8_t)(w / 2);
	
	uint8_t baseHue = animCfg.hue;
	for (int x = 0; x < w; ++x) {
		int dist = abs(x - (int)centerX);
		uint8_t wave1 = sin8((uint8_t)t + (dist * 4));
		uint8_t wave2 = sin8((uint8_t)(t + 128) + (dist * 4));
		uint8_t combined = qadd8(scale8(wave1, 200), scale8(wave2, 100));
		
		for (int y = 0; y < hgt; ++y) {
			uint8_t vOut = scale8(PULSEWAVE_DEFAULT_VAL, combined);
			uint8_t hOut = (uint8_t)(baseHue + (y == 0 ? 0 : 16));
			matrix.setPixelHSV(x, y, hOut, PULSEWAVE_DEFAULT_SAT, vOut);
		}
	}
}
