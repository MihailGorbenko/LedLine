#include "AppController.hpp"

AppController::AppController(LedMatrix& m, RotaryEncoder& enc)
	: matrix(&m),
	  encoder(&enc),
	  currentIndex(0),
	  mode(MODE_SELECT_ANIM),
	  powered(true),
	  powered_off_shown(false),
	  brightStep(APP_STEPS/2),
	  colorStep(0),
	  btnDown(false),
	  btnPressedMillis(0),
	  powerOffAnim(m),
	  lastFrameMillis(0),
	  frameIntervalMs(0) {
	// clamp to avoid 0ms interval on misconfigured APP_FPS
	unsigned long interval = (APP_FPS > 0) ? (1000UL / (unsigned long)APP_FPS) : 33UL;
	if (interval == 0) interval = 1;
	frameIntervalMs = interval;
}

void AppController::addAnimation(AnimationBase* a) {
	if (!a) return;
	animations.push_back(a);
}

void AppController::begin() {
	encoder->attachListener(this);
	encoder->init();

	// configure accel from controller (x3 on fast)
	encoder->setAccelThresholds(100, 40);
	encoder->setAccelMultipliers(2, 3);
	encoder->setAccelEnabled(true);

	loadState();
	applyMasterBrightness();

	// sync encoder to current mode/value so acceleration affects controller logic
	if (!animations.empty()) {
		encoder->setBoundaries(0, (int)animations.size() - 1, true);
		encoder->setValue(currentIndex);
	} else {
		encoder->setBoundaries(0, 0, false);
		encoder->setValue(0);
	}

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

	if (btnDown && powered) {
		unsigned long held = now - btnPressedMillis;
		if (held >= APP_POWEROFF_HOLD_MS) {
			if (!matrix) return;  // safety check
			powerOffAnim.setProgress(0);

			// show final frame, then power off
			matrix->clear();
			powerOffAnim.render();
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
			// clear matrix completely after power-off sequence
			matrix->clear();
			matrix->show();
		} else {
			int prog = 255 - (int)((uint32_t)held * 255 / APP_POWEROFF_HOLD_MS);
			if (prog < 0) prog = 0;
			powerOffAnim.setProgress((uint8_t)prog);

			// ensure it's visible while holding
			matrix->clear();
			powerOffAnim.render();
			matrix->show();
		}
		return; // while holding, ignore other rendering
	}

	if (!powered) {
		// keep matrix cleared while powered off (only once, not every cycle)
		if (!powered_off_shown && matrix) {
			matrix->clear();
			matrix->show();
			powered_off_shown = true;
		}
		return;
	}

	if (animations.empty()) return;
	// Safe frame timing: protect against millis() wrap (~49 days on 32-bit)
	unsigned long elapsed = (unsigned long)(now - lastFrameMillis);
	if (elapsed < frameIntervalMs) return;
	lastFrameMillis = now;

	// bounds-checked animation render
	if (currentIndex >= 0 && currentIndex < (int)animations.size()) {
		animations[currentIndex]->render();
	}
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
			if (held < APP_POWEROFF_HOLD_MS) {
				powered = true;
				powered_off_shown = false;  // reset flag for next power-off
				loadState();
				if (matrix) {
					// clear screen before restoring animation
					matrix->clear();
					matrix->show();
				}
				applyMasterBrightness();
				mode = MODE_SELECT_ANIM;

				// restore current animation settings (if any)
				if (!animations.empty() && currentIndex >= 0 && currentIndex < (int)animations.size()) {
					char key[32];
					snprintf(key, sizeof(key), "anim_%d", currentIndex);
					animations[currentIndex]->loadFromNVS(key);
				}

				// sync encoder after power-on
				if (!animations.empty()) {
					encoder->setBoundaries(0, (int)animations.size() - 1, true);
					encoder->setValue(currentIndex);
				} else {
					encoder->setBoundaries(0, 0, false);
					encoder->setValue(0);
				}
			}
			return;
		}

		if (held >= APP_POWEROFF_HOLD_MS) return;

		// short press: cycle modes
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

		// sync encoder to new mode/value (so accel affects correct parameter)
		switch (mode) {
			case MODE_SELECT_ANIM:
				if (!animations.empty()) {
					encoder->setBoundaries(0, (int)animations.size() - 1, true);
					encoder->setValue(currentIndex);
				}
				break;
			case MODE_BRIGHTNESS:
				encoder->setBoundaries(0, APP_STEPS - 1, false);
				encoder->setValue(brightStep);
				break;
			case MODE_COLOR:
				encoder->setBoundaries(0, APP_STEPS - 1, false);
				encoder->setValue(colorStep);
				break;
			case MODE_POWEROFF:
				break;
		}
		return;
	}

	// rotation: use encoder 'value' (acceleration now works)
	if (ev == RotaryEncoder::INCREMENT || ev == RotaryEncoder::DECREMENT) {
		if (!powered || btnDown) return;

		switch (mode) {
			case MODE_SELECT_ANIM:
				if (animations.empty() || !matrix || !encoder) return;
				{
					int newIndex = constrain(value, 0, (int)animations.size() - 1);
					if (newIndex == currentIndex) return;

					matrix->clear();
					matrix->show();

					currentIndex = newIndex;
					char key[32];
					snprintf(key, sizeof(key), "anim_%d", currentIndex);
					animations[currentIndex]->loadFromNVS(key);
				}
				break;

			case MODE_BRIGHTNESS:
				brightStep = constrain(value, 0, APP_STEPS - 1);
				applyMasterBrightness();
				break;

			case MODE_COLOR:
				colorStep = constrain(value, 0, APP_STEPS - 1);
				if (!matrix || animations.empty()) break;
				if (currentIndex < 0 || currentIndex >= (int)animations.size()) break;
				{
					int delta = (colorStep * 256) / APP_STEPS;
					if (delta > 255) delta = 255;  // clamp to valid hue range
					animations[currentIndex]->setColorHSV((uint8_t)delta, 255, 255);
				}
				break;

			case MODE_POWEROFF:
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
	int bs = constrain(brightStep, 0, APP_STEPS - 1);
	unsigned long v = (unsigned long)map(bs, 0, APP_STEPS - 1, 16, 255);
	if (v > 255) v = 255;  // ensure no overflow
	matrix->setMasterBrightness((uint8_t)v);
}