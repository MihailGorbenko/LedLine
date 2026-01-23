#include "ProgressAnimation.hpp"

ProgressAnimation::ProgressAnimation(LedMatrix& m)
	: AnimationBase(m, PROGRESS_DEFAULT_HUE, PROGRESS_DEFAULT_SAT, PROGRESS_DEFAULT_VAL),
	  totalSeg(1), currentSeg(0) {}

void ProgressAnimation::setColorHSV(uint8_t, uint8_t, uint8_t) {
	// Keep fixed blue look; global masterBrightness will scale
	hue = PROGRESS_DEFAULT_HUE;
	sat = PROGRESS_DEFAULT_SAT;
	val = PROGRESS_DEFAULT_VAL;
}

void ProgressAnimation::setSegments(uint8_t total, uint8_t current) {
	if (total == 0) total = 1;
	totalSeg = total;
	if (current >= totalSeg) current = totalSeg - 1;
	currentSeg = current;
}

void ProgressAnimation::render() {
	if (!matrix) return;
	int w = matrix->width();
	int hgt = matrix->height();
	if (w <= 0) w = 1;
	if (hgt <= 0) hgt = 1;

	// Black background
	matrix->clear();

	// Only the active segment is blue; others remain black
	int segStart = (int)((long)currentSeg * w / (long)totalSeg);
	int segEnd   = (int)((long)(currentSeg + 1) * w / (long)totalSeg) - 1;
	if (segEnd < segStart) segEnd = segStart;
	if (segEnd >= w) segEnd = w - 1;
	for (int x = segStart; x <= segEnd; ++x) {
		for (int y = 0; y < hgt; ++y) {
			matrix->setPixelHSV(x, y, hue, sat, val);
		}
	}

	matrix->show();
}
