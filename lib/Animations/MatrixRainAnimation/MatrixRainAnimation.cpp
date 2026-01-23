#include "MatrixRainAnimation.hpp"

MatrixRainAnimation::MatrixRainAnimation(LedMatrix& m)
	: AnimationBase(m, MATRIXRAIN_DEFAULT_HUE, MATRIXRAIN_DEFAULT_SAT, MATRIXRAIN_DEFAULT_VAL) {
	memset(cols, 0, sizeof(cols));
	memset(intensity, 0, sizeof(intensity));
}

void MatrixRainAnimation::setColorHSV(uint8_t h, uint8_t s, uint8_t v) {
	// Matrix retro: green is signature, but allow customization
	AnimationBase::setColorHSV(h, s, v);
}

void MatrixRainAnimation::render() {
	if (!matrix) return;
	int w = matrix->width();
	int hgt = matrix->height();
	if (w <= 0) w = 1;
	w = min(w, MAX_W);

	matrix->clear();

	// Update rain columns
	for (int x = 0; x < w; ++x) {
		cols[x] = (cols[x] + 1) % hgt;
		// Random new intensity
		if ((uint8_t)random(0, 255) < 32) {
			intensity[x] = (uint8_t)random(100, 255);
		} else {
			intensity[x] = scale8(intensity[x], 220);  // fade out
		}
	}

	// Render rain streaks
	for (int x = 0; x < w; ++x) {
		for (int y = 0; y < hgt; ++y) {
			// Distance from "head" of rain
			int dist = abs((int)y - (int)cols[x]);
			uint8_t streak = (dist == 0) ? 255 : (dist == 1) ? 180 : 0;
			uint8_t vOut = scale8(val, scale8(intensity[x], streak));
			if (vOut > 0) {
				uint8_t hOut = (uint8_t)(hue + (uint8_t)random(0, 24));  // slight hue variation
				matrix->setPixelHSV(x, y, hOut, sat, vOut);
			}
		}
	}

	matrix->show();
}
