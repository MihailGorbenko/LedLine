#include "ScannerAnimation.hpp"
#include <FastLED.h>

ScannerAnimation::ScannerAnimation(uint16_t id, LedMatrix& m)
	: AnimationBase(SCANNER_DEFAULT_HUE, id, m),
	  tail(255) {}

void ScannerAnimation::render() {
	if (!isInitialized()) return;
	matrix.clear();

	int w = mw;
	int hgt = mh;

	// Ping-pong position (triangle wave)
	const uint16_t periodMs = 18; // smaller = faster
	uint32_t step = millis() / periodMs;
	uint32_t span = (uint32_t)(w - 1);
	uint32_t phase = step % (2 * span);
	int head = (phase <= span) ? (int)phase : (int)(2 * span - phase);

	uint8_t baseHue = getConfig().hue;
	for (int x = 0; x < w; ++x) {
		int dist = abs(x - head);
		uint8_t fall = qsub8(255, (uint8_t)min(255, dist * 32));
		uint8_t vOut = scale8(SCANNER_DEFAULT_VAL, scale8(fall, tail));
		if (vOut == 0) continue;

		for (int y = 0; y < hgt; ++y) {
			uint8_t rowMask = (hgt >= 2) ? (uint8_t)((y == (head & 1)) ? 255 : 170) : 255;
			matrix.setPixelHSV(x, y, baseHue, SCANNER_DEFAULT_SAT, scale8(vOut, rowMask));
		}
	}
}
