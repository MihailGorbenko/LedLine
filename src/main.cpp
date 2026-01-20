#include <Arduino.h>
#include <Preferences.h>
#include "../lib/LedMatrix/LedMatrix.hpp"
#include "../lib/RotaryEncoder/RotaryEncoder.hpp"
#include "../lib/Animations/StarsAnimation/StarsAnimation.hpp"
#include "../lib/Animations/GradientAnimation/GradientAnimation.hpp"
#include "../lib/Animations/PowerOffAnimation/PowerOffAnimation.hpp"
#include "../lib/AppController/AppController.hpp"

// глобальные компоненты
LedMatrix matrix;
RotaryEncoder rotary;
GradientAnimation gradient(matrix);
StarsAnimation stars(matrix);
AppController app(matrix, rotary);

void setup() {
	Serial.begin(115200);
	while (!Serial) { delay(10); }
	Serial.println("Starting LedLine...");

	// init NVS (Preferences)
	{
		Preferences prefs;
		prefs.begin("app", false);
		prefs.end();
	}

	// init hardware
	matrix.init();

	// rotary setup
	rotary.init();
	rotary.setSteps(1);
	rotary.setBoundaries(0, 127, false);

	// register animations
	app.addAnimation(&gradient);
	app.addAnimation(&stars);

	// start controller (attaches to rotary)
	app.begin();

	Serial.println("Setup complete.");
}

void loop() {
	// poll encoder (produces events to AppController)
	rotary.update();

	// update app (renders animations / handles poweroff)
	app.update();
}
