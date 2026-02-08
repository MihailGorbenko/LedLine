#include "MatrixRainAnimation.hpp"

MatrixRainAnimation::MatrixRainAnimation(uint16_t id, LedMatrix& m)
	: AnimationBase(MATRIXRAIN_DEFAULT_HUE, id, m) {
	memset(cols, 0, sizeof(cols));
	memset(intensity, 0, sizeof(intensity));
}

void MatrixRainAnimation::onActivate() {
	AnimationBase::onActivate();
	// Cache matrix size and reset columns
	mw = matrix.getWidth();
	mh = matrix.getHeight();
	int w = min(mw, MAX_W);
	if (w < 0) w = 0;
	for (int x = 0; x < w; ++x) {
		cols[x] = 0;
		intensity[x] = 0;
	}
}

void MatrixRainAnimation::render() {
	if (!isInitialized()) return;
	int w = min(mw, MAX_W);
	int hgt = mh;

	matrix.clear();

	// Update rain columns
	for (int x = 0; x < w; ++x) {
		cols[x] = (uint8_t)((cols[x] + 1) % max(1, hgt));
		// Random new intensity
		if ((uint8_t)random(0, 255) < 32) {
			intensity[x] = (uint8_t)random(100, 255);
		} else {
			intensity[x] = scale8(intensity[x], 220);  // fade out
		}
	}

	uint8_t baseHue = getConfig().hue;
	// Render rain streaks
	for (int x = 0; x < w; ++x) {
		for (int y = 0; y < hgt; ++y) {
			int dist = abs((int)y - (int)cols[x]);
			uint8_t streak = (dist == 0) ? 255 : (dist == 1) ? 180 : 0;
			uint8_t vOut = scale8(MATRIXRAIN_DEFAULT_VAL, scale8(intensity[x], streak));
			if (vOut > 0) {
				uint8_t hOut = (uint8_t)(baseHue + (uint8_t)random(0, 24));  // slight hue variation
				matrix.setPixelHSV(x, y, hOut, MATRIXRAIN_DEFAULT_SAT, vOut);
			}
		}
	}

}
