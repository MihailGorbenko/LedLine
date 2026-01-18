#include <Arduino.h>
#include "LedMatrix.hpp"
#include "AiEsp32RotaryEncoder.h"

#define ROTARY_CLK 4
#define ROTARY_DT 3
#define ROTARY_SW 5
#define ROTARY_STEPS 1

LedMatrix matrix;
AiEsp32RotaryEncoder rotary(ROTARY_CLK, ROTARY_DT, ROTARY_SW, -1, ROTARY_STEPS);

uint8_t currentBrightness = 128;
uint8_t hueOffset = 0;
float hueOffsetFloat = 0.0f; // плавное смещение для перелива
unsigned long lastMillis = 0;
const unsigned long refreshMs = 50;

// добавлено: режим звёздного неба и фазы
bool isStarMode = false;
float starPhase[NUM_LEDS];

void IRAM_ATTR readEncoderISR() {
	rotary.readEncoder_ISR();
}

// добавлено: инициализация фаз для звезд
void initStars() {
	for (int i = 0; i < NUM_LEDS; ++i) {
		starPhase[i] = (float)random(0, 628) / 100.0f; // 0..~6.28 (0..2PI)
	}
}

void setup() {
    pinMode(ROTARY_CLK,INPUT_PULLUP);
    pinMode(ROTARY_DT,INPUT_PULLUP);
    pinMode(ROTARY_SW,INPUT_PULLUP);
	Serial.begin(115200);
	while (!Serial) { delay(10); }
	Serial.println("LedMatrix test (encoder). Rotate to change, press to toggle star mode.");

	matrix.init();

	// Rotary init
	rotary.begin();
	rotary.setup(readEncoderISR);
	rotary.setBoundaries(0, 127, false); // 0..127 steps (будет маппиться в 0..255), без wrap
	rotary.setAcceleration(1);
	currentBrightness = 64;
	rotary.setEncoderValue(currentBrightness / 2); // стартовое значение под новую шкалу
	matrix.setMasterBrightness(currentBrightness);

	// init stars buffer (not active until toggled)
	initStars();
}

void loop() {
	// Encoder control
	if (rotary.encoderChanged()) {
		int val = rotary.readEncoder();
		int mapped = constrain(val * 2, 0, 255); // двойной шаг: шаг энкодера -> яркость*2
		currentBrightness = (uint8_t)mapped;
		matrix.setMasterBrightness(currentBrightness);
		Serial.print("Brightness: "); Serial.println(currentBrightness);
	}

	// Изменено: при клике переключаем режим (градиент <-> звёзды)
	if (rotary.isEncoderButtonClicked()) {
		isStarMode = !isStarMode;
		if (isStarMode) {
			initStars();
			Serial.println("Star mode ON");
		} else {
			Serial.println("Star mode OFF");
		}
		// не меняем значение энкодера при переключении, сохраняем яркость
	}

	// Animation timing
	if (millis() - lastMillis < refreshMs) return;
	lastMillis = millis();

	// advance smooth offset (fractional) for shimmer — ускорено для более активного перелива
	hueOffsetFloat += 1.5f; // скорость перелива увеличена
	if (hueOffsetFloat >= 256.0f) hueOffsetFloat -= 256.0f;
	hueOffset = (uint8_t)hueOffsetFloat;

	int w = matrix.getWidth();
	int h = matrix.getHeight();

	if (isStarMode) {
		// звёздное небо сереневого/лилового цвета
		for (int y = 0; y < h; ++y) {
			for (int x = 0; x < w; ++x) {
				int idx = y * w + x;
				// обновляем фазу и рассчитываем пульсацию
				starPhase[idx] += 0.06f + (sinf((float)x * 0.05f + hueOffsetFloat * 0.01f) * 0.02f);
				if (starPhase[idx] > 2 * PI) starPhase[idx] -= 2 * PI;
				float pulse = 0.15f + 0.85f * (0.5f + 0.5f * sinf(starPhase[idx])); // 0..1
				uint8_t v = (uint8_t)constrain(pulse * 255.0f, 0.0f, 255.0f);
				// hue для сереневого оттенка (примерно 150..180) — небольшая локальная модуляция
				float huef = 170.0f + sinf(starPhase[idx] * 0.7f) * 8.0f;
				uint8_t hue = (uint8_t)huef;
				matrix.setPixelHSV(x, y, hue, 255, v);
			}
		}
	} else {
		// существующий градиент с усиленным шимером
		for (int y = 0; y < h; ++y) {
			for (int x = 0; x < w; ++x) {
				float baseHueF = (float)(x) * (256.0f / (float)w) + hueOffsetFloat;
				float shimmer = sinf((x * 0.25f) + (y * 1.2f) + (hueOffsetFloat * 0.06f)) * 40.0f;
				int hueInt = (int)roundf(baseHueF + shimmer);
				uint8_t hue = (uint8_t)hueInt;
				matrix.setPixelHSV(x, y, hue, 255, 255);
			}
		}
	}

	matrix.update();

	// advance integer offset as well (for alternatives)
	// hueOffset++;
}
