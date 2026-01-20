#include "GradientAnimation.hpp"

GradientAnimation::GradientAnimation(LedMatrix& m)
	: AnimationBase(m, GRADIENT_DEFAULT_HUE, GRADIENT_DEFAULT_SAT, GRADIENT_DEFAULT_VAL),
	  hueSpan(96), reverse(false) {
	// пусто
}

void GradientAnimation::setColorHSV(uint8_t h, uint8_t s, uint8_t v) {
	// заглушка: сохраняем базовые параметры, но render будет генерировать динамический градиент
	AnimationBase::setColorHSV(h, s, v);
}

bool GradientAnimation::saveColor(const char* key) {
	Serial.print("GradientAnimation: saveColor - stub, not saved for key: ");
	Serial.println(key ? key : "(null)");
	return false;
}

bool GradientAnimation::loadColor(const char* key) {
	Serial.print("GradientAnimation: loadColor - stub, not loaded for key: ");
	Serial.println(key ? key : "(null)");
	return false;
}

uint8_t GradientAnimation::computeHueForPos(unsigned long now, int pos, int total) const {
	if (total <= 0) return hue;
	uint16_t posHue = (uint32_t(pos) * 256) / (uint32_t)max(1, total);
	uint16_t timeShift = (now / 20) & 0xFF;
	int h = (int)hue + (reverse ? -1 : 1) * ((posHue * hueSpan) / 256) + timeShift;
	return (uint8_t)(h & 0xFF);
}

void GradientAnimation::render() {
	unsigned long now = millis();
	matrix->clear();

	// получить размеры матрицы из объекта (fallback 8x8)
	int w = 8;
	int hgt = 8;
	if (matrix) {
		int mw = matrix->width();
		int mh = matrix->height();
		if (mw > 0) w = mw;
		if (mh > 0) hgt = mh;
	}
	int total = w; // градиент по X

	for (int x = 0; x < w; ++x) {
		uint8_t colHue = computeHueForPos(now, x, total);
		for (int y = 0; y < hgt; ++y) {
			matrix->setPixelHSV(x, y, colHue, sat, val);
		}
	}

	matrix->show();
}
