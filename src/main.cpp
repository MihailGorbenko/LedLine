#include <Arduino.h>
#include <Preferences.h>
#include "../lib/LedMatrix/LedMatrix.hpp"
#include "../lib/RotaryEncoder/RotaryEncoder.hpp"
#include "../lib/Animations/StarsAnimation/StarsAnimation.hpp"
#include "../lib/Animations/GradientAnimation/GradientAnimation.hpp"
#include "../lib/Animations/RainbowChaseAnimation/RainbowChaseAnimation.hpp"
#include "../lib/Animations/ScannerAnimation/ScannerAnimation.hpp"
#include "../lib/Animations/PlasmaAnimation/PlasmaAnimation.hpp"
#include "../lib/Animations/ConfettiAnimation/ConfettiAnimation.hpp"
#include "../lib/Animations/SparkleWaveAnimation/SparkleWaveAnimation.hpp"
#include "../lib/Animations/PulseWaveAnimation/PulseWaveAnimation.hpp"
#include "../lib/Animations/MatrixRainAnimation/MatrixRainAnimation.hpp"
#include "../lib/Animations/CosmicDustAnimation/CosmicDustAnimation.hpp"
#include "../lib/Animations/NeonGridAnimation/NeonGridAnimation.hpp"
#include "../lib/Animations/GalacticWarpAnimation/GalacticWarpAnimation.hpp"
#include "../lib/Animations/PowerOffAnimation/PowerOffAnimation.hpp"
#include "../lib/AppController/AppController.hpp"

// глобальные компоненты
LedMatrix matrix;
RotaryEncoder rotary;
GradientAnimation gradient(matrix);
StarsAnimation stars(matrix);
RainbowChaseAnimation rainbow(matrix);
ScannerAnimation scanner(matrix);
PlasmaAnimation plasma(matrix);
ConfettiAnimation confetti(matrix);
SparkleWaveAnimation sparkleWave(matrix);
PulseWaveAnimation pulseWave(matrix);
MatrixRainAnimation matrixRain(matrix);
CosmicDustAnimation cosmicDust(matrix);
NeonGridAnimation neonGrid(matrix);
GalacticWarpAnimation galacticWarp(matrix);
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

	// register animations
	app.addAnimation(&gradient);
	app.addAnimation(&stars);
	app.addAnimation(&rainbow);
	app.addAnimation(&scanner);
	app.addAnimation(&plasma);
	app.addAnimation(&confetti);
	app.addAnimation(&sparkleWave);
	app.addAnimation(&pulseWave);
	app.addAnimation(&matrixRain);
	app.addAnimation(&cosmicDust);
	app.addAnimation(&neonGrid);
	app.addAnimation(&galacticWarp);

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
