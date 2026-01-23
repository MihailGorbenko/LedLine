#include "ConfettiAnimation.hpp"
#include <FastLED.h>

ConfettiAnimation::ConfettiAnimation(LedMatrix& m)
	: AnimationBase(m, CONFETTI_DEFAULT_HUE, CONFETTI_DEFAULT_SAT, CONFETTI_DEFAULT_VAL),
	  vbuf{0},
	  hbuf{0} {
}

void ConfettiAnimation::setColorHSV(uint8_t h, uint8_t s, uint8_t v) {
	// Confetti: hue is base, sat and val control sparkle intensity
	AnimationBase::setColorHSV(h, s, v);
}

void ConfettiAnimation::render() {
	if (!matrix) return;
	int w = matrix->width();
	int hgt = matrix->height();
	if (w <= 0) w = 1;
	if (hgt <= 0) hgt = 1;
	w = min(w, MAX_W);
	hgt = min(hgt, MAX_H);

	// Fade out existing confetti
	for (int y = 0; y < hgt; ++y) {
		for (int x = 0; x < w; ++x) {
			int idx = y * MAX_W + x;
			vbuf[idx] = scale8(vbuf[idx], 220); // decay
		}
	}

	// Add a couple of random sparkles per frame
	for (int i = 0; i < 2; ++i) {
		int x = random(0, w);
		int y = random(0, hgt);
		int idx = y * MAX_W + x;
		vbuf[idx] = qadd8(vbuf[idx], (uint8_t)random(80, 200));
		// random hue around base hue
		hbuf[idx] = (uint8_t)(hue + (uint8_t)random(0, 96));
	}

	matrix->clear();
	for (int y = 0; y < hgt; ++y) {
		for (int x = 0; x < w; ++x) {
			int idx = y * MAX_W + x;
			uint8_t vOut = scale8(val, vbuf[idx]);
			if (vOut == 0) continue;
			matrix->setPixelHSV(x, y, hbuf[idx], sat, vOut);
		}
	}

	matrix->show();
}
