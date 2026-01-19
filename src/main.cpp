#include <Arduino.h>
#include "LedMatrix.hpp"
#include "RotaryEncoder.hpp" // заменили библиотеку

#define ROTARY_STEPS 1

LedMatrix matrix;
RotaryEncoder rotary; // теперь без параметров

uint8_t currentBrightness = 16;
uint8_t hueOffset = 0;
float hueOffsetFloat = 0.0f; // плавное смещение для перелива
unsigned long lastMillis = 0;
const unsigned long refreshMs = 50;

// button / mode change state
bool colorAdjustMode = false; // true = вращение меняет цвет, краткое нажатие выходит в режим выбора
bool colorLocked = false; // true = пользователь подтвердил и цвет должен быть зафиксирован

// режимы: 0 = gradient, 1 = stars, 2 = lamp ignition, 3 = blue gas discharge
int mode = 0;

// звёздные параметры (существовали)
bool isStarMode = false; // (необязательно, можно использовать mode==1)
float starPhase[NUM_LEDS];
float starAmp[NUM_LEDS];
float starSpeed[NUM_LEDS];
uint8_t starSpikeTimer[NUM_LEDS];

// добавлено: параметры лампы
float lampNoise[NUM_LEDS];
unsigned long lampStartMillis = 0;
unsigned long lampDuration = 4000; // ms до стабильного свечения
bool lampStable = false;

// добавлено: параметры газового разряда
float gasEnergy[NUM_LEDS];
float gasAdd[NUM_LEDS];

// добавлено: инициализация фаз/амплитуд/скоростей/таймеров для звезд
void initStars() {
	for (int i = 0; i < NUM_LEDS; ++i) {
		starPhase[i] = (float)random(0, 628) / 100.0f; // 0..~6.28 (0..2PI)
		starAmp[i] = (float)random(20, 100) / 100.0f; // 0.20 .. 0.99 - индивидуальная амплитуда
		starSpeed[i] = (float)random(80, 160) / 100.0f; // 0.80 .. 1.60 - индивидуальная скорость
		starSpikeTimer[i] = 0;
	}
	randomSeed(micros());
}

// добавлено: инициализация лампы
void initLamp() {
	for (int i = 0; i < NUM_LEDS; ++i) {
		lampNoise[i] = ((float)random(-100, 100)) / 100.0f; // небольшая пространственная вариация
	}
	lampStartMillis = millis();
	lampStable = false;
}

void initGas() {
	for (int i = 0; i < NUM_LEDS; ++i) {
		gasEnergy[i] = 0.0f;
		gasAdd[i] = 0.0f;
	}
	randomSeed(micros());
}

// Listener: адаптер между событием RotaryEncoder и текущей логикой
class MainRotaryListener : public RotaryEncoder::IEncoderListener {
public:
    void onEvent(RotaryEncoder::Event ev, int value) override {
        if (ev == RotaryEncoder::INCREMENT || ev == RotaryEncoder::DECREMENT) {
            int mapped = constrain(value * 2, 0, 255);
            if (colorAdjustMode) {
                hueOffset = (uint8_t)mapped;
                hueOffsetFloat = (float)hueOffset;
                Serial.print("Hue: "); Serial.println(hueOffset);
            } else {
                currentBrightness = (uint8_t)mapped;
                matrix.setMasterBrightness(currentBrightness);
                Serial.print("Brightness: "); Serial.println(currentBrightness);
            }
        } else if (ev == RotaryEncoder::PRESS) {
            if (colorAdjustMode) {
                colorAdjustMode = false;
                colorLocked = true;
                rotary.setValue(currentBrightness / 2);
                Serial.println("Exited Color Adjust Mode -> Selection Mode");
            } else {
                mode = (mode + 1) % 4;
                if (mode == 1) { initStars(); Serial.println("Mode: Stars"); }
                else if (mode == 2) { initLamp(); Serial.println("Mode: Lamp ignition"); }
                else if (mode == 3) { initGas(); Serial.println("Mode: Blue Gas Discharge"); }
                else { Serial.println("Mode: Gradient"); }
            }
        } else if (ev == RotaryEncoder::LONG_PRESS) {
            colorAdjustMode = true;
            colorLocked = false;
            rotary.setValue(hueOffset / 2);
            Serial.println("Entered Color Adjust Mode (long press)");
            Serial.println("переход в режим выбора цвета");
        }
    }
} mainRotaryListener;

void setup() {
	Serial.begin(115200);
	while (!Serial) { delay(10); }
	Serial.println("LedMatrix test (encoder). Rotate to change, press to cycle modes (Gradient, Stars, Lamp, Gas).");

	matrix.init();

	// Rotary init (pins инициализируются внутри rotary.init())
	rotary.init();
	rotary.setSteps(ROTARY_STEPS); // установка шага через метод
	rotary.setBoundaries(0, 127, false); // 0..127 steps (будет маппиться в 0..255), без wrap
	currentBrightness = 16;
	rotary.setValue(currentBrightness / 2); // стартовое значение под новую шкалу
	matrix.setMasterBrightness(currentBrightness);
	rotary.attachListener(&mainRotaryListener);

	// init stars & lamp buffer (not active until toggled)
	initStars();
	initLamp();
	initGas();
}

void loop() {
    // process rotary (поллинг и уведомления)
    rotary.update();

	// Animation timing
	if (millis() - lastMillis < refreshMs) return;
	lastMillis = millis();

	// advance smooth offset (fractional) for shimmer
	// не двигать автоматический сдвиг цвета, когда пользователь в режиме настройки цвета
	// но позволить градиенту плавать, если цвет зафиксирован — используем hueOffsetFloat отдельно
	if (!colorAdjustMode) {
		hueOffsetFloat += 1.5f;
		if (hueOffsetFloat >= 256.0f) hueOffsetFloat -= 256.0f;
	}
	// если цвет НЕ зафиксирован — синхронизируем основной hue с плавающим
	if (!colorLocked) {
		hueOffset = (uint8_t)hueOffsetFloat;
	}
	// если colorLocked == true, hueOffset остаётся выбранным пользователем,
	// а hueOffsetFloat продолжает плавать и используется для градиента

	// базовый hue для эффектов (если цвет зафиксирован - используем hueOffset, иначе - плавающий)
	uint8_t baseHueForEffects = colorLocked ? hueOffset : (uint8_t)hueOffsetFloat;

	int w = matrix.getWidth();
	int h = matrix.getHeight();

	if (mode == 1) {
		// star mode
		for (int y = 0; y < h; ++y) {
			for (int x = 0; x < w; ++x) {
				int idx = y * w + x;
				starPhase[idx] += 0.04f * starSpeed[idx] + (sinf((float)x * 0.05f + hueOffsetFloat * 0.01f) * 0.01f);
				if (starPhase[idx] > 2 * PI) starPhase[idx] -= 2 * PI;

				if (starSpikeTimer[idx] == 0) {
					if (random(0, 1000) < 3) {
						starSpikeTimer[idx] = (uint8_t)random(1, 5);
					}
				}

				float pulse;
				if (starSpikeTimer[idx] > 0) {
					pulse = 1.0f;
					starSpikeTimer[idx]--;
				} else {
					float base = 0.5f + 0.5f * sinf(starPhase[idx]);
					float sharp = powf(base, 6.0f);
					pulse = 0.02f + starAmp[idx] * sharp;
					if (pulse > 1.0f) pulse = 1.0f;
				}

				uint8_t v = (uint8_t)constrain(pulse * 255.0f, 0.0f, 255.0f);
				// use baseHueForEffects + small modulation
				int hueMod = (int)roundf(sinf(starPhase[idx] * 0.7f) * 6.0f);
				uint8_t hue = (uint8_t)((int)baseHueForEffects + hueMod);
				matrix.setPixelHSV(x, y, hue, 255, v);
			}
		}
	} else if (mode == 2) {
		// lamp ignition mode
		for (int y = 0; y < h; ++y) {
			for (int x = 0; x < w; ++x) {
				int idx = y * w + x;
				// use base hue for lamp
				uint8_t hue = baseHueForEffects;
				uint8_t sat = 60;
				// flicker strength decreases as progress -> 0
				float flickerBase = 1.0f - (float)(millis() - lampStartMillis) / (float)lampDuration;
				if (flickerBase < 0) flickerBase = 0;

				// spatial noise + occasional spike for ignition
				float n = lampNoise[idx];
				float rnd = (float)random(-100, 100) / 100.0f;
				// transient flicker component
				float flick = flickerBase * (0.5f * rnd + 0.5f * sinf((float)idx * 0.37f + hueOffsetFloat * 0.12f + n));
				// occasional ignition spike early in progress
				float spike = 0.0f;
				if (!lampStable && random(0, 100) < 5) spike = (1.0f - (float)(millis() - lampStartMillis) / (float)lampDuration) * ((float)random(30, 150) / 100.0f);

				// compute V: ramp to target (e.g., 220) plus flicker and spikes
				float targetV = 220.0f * (float)(millis() - lampStartMillis) / (float)lampDuration + 80.0f * (1.0f - (float)(millis() - lampStartMillis) / (float)lampDuration); // initial dim -> target bright
				float vF = targetV + (flick * 120.0f) + (spike * 255.0f);
				// clamp
				if (vF < 0.0f) vF = 0.0f;
				if (vF > 255.0f) vF = 255.0f;
				uint8_t v = (uint8_t)vF;

				matrix.setPixelHSV(x, y, hue, sat, v);
			}
		}
	} else if (mode == 3) {
		// gas mode
		for (int y = 0; y < h; ++y) {
			for (int x = 0; x < w; ++x) {
				int idx = y * w + x;

				// natural decay
				gasEnergy[idx] *= 0.92f;

				// occasional random excitation (breakdown seeds)
				if (random(0, 1000) < 6) { // tweak probability
					gasEnergy[idx] += (float)random(120, 255) / 255.0f;
				}

				// clamp
				if (gasEnergy[idx] > 1.5f) gasEnergy[idx] = 1.5f;

				// if energy high - bright discharge, propagate to neighbors
				if (gasEnergy[idx] > 0.55f) {
					uint8_t v = (uint8_t)constrain(gasEnergy[idx] * 255.0f, 0.0f, 255.0f);
					uint8_t hue = baseHueForEffects;
					matrix.setPixelHSV(x, y, hue, 240, v);

					// propagate fraction to neighbors
					float prop = gasEnergy[idx] * 0.5f;
					if (x > 0) gasAdd[idx - 1] += prop * 0.6f;
					if (x < w - 1) gasAdd[idx + 1] += prop * 0.6f;
					if (y > 0) gasAdd[idx - w] += prop * 0.5f;
					if (y < h - 1) gasAdd[idx + w] += prop * 0.5f;
				} else {
					uint8_t v = (uint8_t)constrain(gasEnergy[idx] * 90.0f, 0.0f, 255.0f);
					uint8_t hue = baseHueForEffects;
					matrix.setPixelHSV(x, y, hue, 200, v);
				}
			}
		}
	} else {
		// gradient mode: продолжает использовать hueOffsetFloat (плавит)
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
