#include "NeonGridAnimation.hpp"

NeonGridAnimation::NeonGridAnimation(uint16_t id, LedMatrix& m)
	: AnimationBase(NEONGRID_DEFAULT_HUE, id, m) {
	memset(gridPulse, 0, sizeof(gridPulse));
}

void NeonGridAnimation::render() {
	if (!isInitialized()) return;
	matrix.clear();

	int w = mw;
	int hgt = mh;
	w = min(w, MAX_W);

	uint16_t t = (uint16_t)(millis() / 8);

	uint8_t baseHue = getConfig().hue;
	// Horizontal grid lines (constant)
	for (int x = 0; x < w; ++x) {
		uint8_t baseIntensity = 255; // allow full brightness
		for (int y = 0; y < hgt; ++y) {
			uint8_t vOut = scale8(NEONGRID_DEFAULT_VAL, baseIntensity);
			uint8_t hOut = (uint8_t)(baseHue + (y * 64));  // hue per row
			matrix.setPixelHSV(x, y, hOut, NEONGRID_DEFAULT_SAT, vOut);
		}
	}

	// Vertical pulse "grid nodes" (moving wave)
	for (int x = 0; x < w; ++x) {
		uint8_t pulseWave = sin8((uint8_t)(t + x * 6));
		uint8_t intensity = scale8(pulseWave, NEONGRID_DEFAULT_VAL);

		for (int y = 0; y < hgt; ++y) {
			uint8_t hOut = (uint8_t)(baseHue + intensity / 2);
			uint8_t vOut = qadd8(scale8(NEONGRID_DEFAULT_VAL, 255), scale8(NEONGRID_DEFAULT_VAL, scale8(pulseWave, 255)));
			vOut = min<uint8_t>(vOut, NEONGRID_DEFAULT_VAL);
			matrix.setPixelHSV(x, y, hOut, NEONGRID_DEFAULT_SAT, vOut);
		}
	}

}
