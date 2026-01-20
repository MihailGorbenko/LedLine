#include "PowerOffAnimation.hpp"

PowerOffAnimation::PowerOffAnimation(LedMatrix& m)
	: matrix(&m),
	  hue(POWEROFF_DEFAULT_HUE),
	  sat(POWEROFF_DEFAULT_SAT),
	  val(POWEROFF_DEFAULT_VAL),
	  progress(0) {
	// пусто
}

void PowerOffAnimation::setProgress(uint8_t p) {
	progress = p;
}

void PowerOffAnimation::setColorHSV(uint8_t h, uint8_t s, uint8_t v) {
	// заглушка: обновляем цвет, но не сохраняем
	hue = h; sat = s; val = v;
}

bool PowerOffAnimation::saveColor(const char* key) {
	Serial.print("PowerOffAnimation: saveColor - stub, not saved for key: ");
	Serial.println(key ? key : "(null)");
	return false;
}

bool PowerOffAnimation::loadColor(const char* key) {
	Serial.print("PowerOffAnimation: loadColor - stub, not loaded for key: ");
	Serial.println(key ? key : "(null)");
	return false;
}

void PowerOffAnimation::render() {
	if (!matrix) return;
	matrix->clear();

	int w = matrix->width();
	int h = matrix->height();
	if (w <= 0) w = 1;
	if (h <= 0) h = 1;

	// заполнение по ширине: filledColumns полные, next колонка частично
	int filledColumns = (uint32_t(progress) * w) / 255;
	int rem = (uint32_t(progress) * w) % 255; // remainder scaled to 255 per column
	for (int x = 0; x < w; ++x) {
		uint8_t colV = 0;
		if (x < filledColumns) {
			colV = val;
		} else if (x == filledColumns) {
			// частичная колонка: rem/255 * val
			colV = (uint32_t(rem) * uint32_t(val)) / 255;
		} else {
			colV = 0;
		}
		for (int y = 0; y < h; ++y) {
			if (colV > 0) matrix->setPixelHSV(x, y, hue, sat, colV);
			// else оставляем выключенным
		}
	}

	matrix->show();
}

