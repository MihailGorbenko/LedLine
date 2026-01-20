#include "AppController.hpp"

AppController::AppController(LedMatrix& m, RotaryEncoder& enc)
	: matrix(&m),
	  encoder(&enc),
	  currentIndex(0),
	  mode(MODE_SELECT_ANIM),
	  powered(true),
	  brightStep(APP_STEPS/2),
	  colorStep(0),
	  btnDown(false),
	  btnPressedMillis(0),
	  powerOffAnim(m),
	  lastFrameMillis(0),
	  frameIntervalMs(1000 / APP_FPS) {
}

void AppController::addAnimation(AnimationBase* a) {
	if (!a) return;
	animations.push_back(a);
}

void AppController::begin() {
	encoder->attachListener(this);
	encoder->init();
	loadState();
	applyMasterBrightness();
	if (!animations.empty()) {
		if (currentIndex < 0 || currentIndex >= (int)animations.size()) currentIndex = 0;
		// try load animation-specific settings (optional)
		char key[32];
		snprintf(key, sizeof(key), "anim_%d", currentIndex);
		animations[currentIndex]->loadFromNVS(key);
	}
}

void AppController::update() {
	unsigned long now = millis();

	// handling power-off hold while button down
	if (btnDown && powered) {
		unsigned long held = now - btnPressedMillis;
		if (held >= APP_POWEROFF_HOLD_MS) {
			// reached threshold -> power off
			powerOffAnim.setProgress(0);
			powerOffAnim.render();
			matrix->clear();
			matrix->show();
			// save current animation color before powering off
			if (!animations.empty() && currentIndex >= 0 && currentIndex < (int)animations.size()) {
				char key[32];
				snprintf(key, sizeof(key), "anim_%d", currentIndex);
				animations[currentIndex]->saveToNVS(key);
			}
			powered = false;
			mode = MODE_POWEROFF;
			saveState();
		} else {
			// show decreasing progress
			int prog = 255 - (int)((uint32_t)held * 255 / APP_POWEROFF_HOLD_MS);
			if (prog < 0) prog = 0;
			powerOffAnim.setProgress((uint8_t)prog);
			powerOffAnim.render();
		}
		return; // while holding, ignore other rendering
	}

	if (!powered) {
		// waiting for short press to turn on (handled in onEvent)
		return;
	}

	if (animations.empty()) return;
	if (now - lastFrameMillis < frameIntervalMs) return;
	lastFrameMillis = now;

	// render current animation
	animations[currentIndex]->render();
}

void AppController::onEvent(RotaryEncoder::Event ev, int value) {
	if (ev == RotaryEncoder::PRESS_START) {
		btnDown = true;
		btnPressedMillis = millis();
		return;
	}

	if (ev == RotaryEncoder::PRESS_END) {
		unsigned long held = millis() - btnPressedMillis;
		btnDown = false;
		if (!powered) {
			// short press -> power on
			if (held < APP_POWEROFF_HOLD_MS) {
				powered = true;
				loadState();
				applyMasterBrightness();
					mode = MODE_SELECT_ANIM;
					// restore current animation color (if any)
					if (!animations.empty() && currentIndex >= 0 && currentIndex < (int)animations.size()) {
						char key[32];
						snprintf(key, sizeof(key), "anim_%d", currentIndex);
						animations[currentIndex]->loadFromNVS(key);
					}
			}
			return;
		}
		if (held >= APP_POWEROFF_HOLD_MS) return; // already handled in update()

		// short press: cycle modes
		// если выходим из режима выбора цвета — сохранить выбранный цвет для текущей анимации
		if (mode == MODE_COLOR) {
			if (!animations.empty() && currentIndex >= 0 && currentIndex < (int)animations.size()) {
				char key[32];
				snprintf(key, sizeof(key), "anim_%d", currentIndex);
				animations[currentIndex]->saveToNVS(key);
			}
			mode = MODE_SELECT_ANIM;
		} else if (mode == MODE_SELECT_ANIM) {
			mode = MODE_BRIGHTNESS;
		} else if (mode == MODE_BRIGHTNESS) {
			mode = MODE_COLOR;
		} else {
			mode = MODE_SELECT_ANIM;
		}
		return;
	}

	// rotation
	if (ev == RotaryEncoder::INCREMENT || ev == RotaryEncoder::DECREMENT) {
		int dir = (ev == RotaryEncoder::INCREMENT) ? 1 : -1;
		if (!powered || btnDown) return;
		switch (mode) {
			case MODE_SELECT_ANIM:
				if (animations.empty()) return;
				{
					// очистить матрицу перед сменой анимации, чтобы не осталось артефактов
					matrix->clear();
					matrix->show();

					int n = (int)animations.size();
					currentIndex = (currentIndex + dir) % n;
					if (currentIndex < 0) currentIndex += n;
					// try load animation-specific settings (optional)
					char key[32];
					snprintf(key, sizeof(key), "anim_%d", currentIndex);
					animations[currentIndex]->loadFromNVS(key);
				}
				// try load animation-specific settings (optional)
				break;
			case MODE_BRIGHTNESS:
				brightStep = constrain(brightStep + dir, 0, APP_STEPS - 1);
				applyMasterBrightness();
				break;
			case MODE_COLOR:
				colorStep = constrain(colorStep + dir, 0, APP_STEPS - 1);
				// interpret colorStep as hue offset and set on current animation
				if (!animations.empty()) {
					int delta = (colorStep * 256) / APP_STEPS;
					animations[currentIndex]->setColorHSV((uint8_t)delta, 255, 255);
				}
				break;
			case MODE_POWEROFF:
				// ignore rotations
				break;
		}
	}
}

bool AppController::saveState() {
	Preferences prefs;
	if (!prefs.begin("app", false)) return false;
	prefs.putUInt("lastAnim", (uint32_t)currentIndex);
	prefs.putUShort("brightStep", (uint16_t)brightStep);
	prefs.end();
	return true;
}

bool AppController::loadState() {
	Preferences prefs;
	if (!prefs.begin("app", true)) {
		currentIndex = 0;
		brightStep = APP_STEPS/2;
		return false;
	}
	uint32_t ai = prefs.getUInt("lastAnim", 0);
	uint16_t bs = prefs.getUShort("brightStep", (uint16_t)(APP_STEPS/2));
	prefs.end();
	currentIndex = (int)ai;
	brightStep = (int)bs;
	if (currentIndex < 0) currentIndex = 0;
	if (brightStep < 0) brightStep = 0;
	if (brightStep >= APP_STEPS) brightStep = APP_STEPS - 1;
	return true;
}

void AppController::applyMasterBrightness() {
	int v = map(brightStep, 0, APP_STEPS - 1, 16, 255);
	// prefer LedMatrix::setMasterBrightness if exists
	matrix->setMasterBrightness((uint8_t)v);
}