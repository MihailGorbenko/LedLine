#include <Arduino.h>
#include "debug.hpp"
#include "../lib/LedMatrix/LedMatrix.hpp"
#include "../lib/RotaryEncoder/RotaryEncoder.hpp"
#include "../lib/Animations/StarsAnimation/StarsAnimation.hpp"
#include "../lib/Animations/RainbowChaseAnimation/RainbowChaseAnimation.hpp"
#include "../lib/Animations/PlasmaAnimation/PlasmaAnimation.hpp"
#include "../lib/Animations/SparkleWaveAnimation/SparkleWaveAnimation.hpp"
#include "../lib/Animations/PulseWaveAnimation/PulseWaveAnimation.hpp"
#include "../lib/Animations/MatrixRainAnimation/MatrixRainAnimation.hpp"
#include "../lib/Animations/NeonGridAnimation/NeonGridAnimation.hpp"
#include "../lib/Animations/ScannerAnimation/ScannerAnimation.hpp"
#include "../lib/Animations/GalacticWarpAnimation/GalacticWarpAnimation.hpp"
#include "../lib/Animations/FluoroLampAnimation/FluoroLampAnimation.hpp"
#include "../lib/AnimationManager/AnimationManager.hpp"
#include "../lib/StorageManager/StorageManager.hpp"
#include "../lib/AppManager/AppManager.hpp"

// Global components
LedMatrix matrix;
RotaryEncoder rotary;
StorageManager storage;
AnimationManager animMgr(storage);
AppManager app(animMgr, rotary, matrix, storage);


// Stable IDs for available animations
#define ANIM_ID_STARS           1
#define ANIM_ID_RAINBOW         2
#define ANIM_ID_PLASMA          3
#define ANIM_ID_SPARKLE_WAVE    4
#define ANIM_ID_PULSE_WAVE      5
#define ANIM_ID_MATRIX_RAIN     6
#define ANIM_ID_NEON_GRID       7
#define ANIM_ID_SCANNER         8
#define ANIM_ID_GALACTIC_WARP   9
#define ANIM_ID_FLUORO_LAMP     10



StarsAnimation         stars(ANIM_ID_STARS, matrix);
RainbowChaseAnimation  rainbow(ANIM_ID_RAINBOW, matrix);
PlasmaAnimation        plasma(ANIM_ID_PLASMA, matrix);
SparkleWaveAnimation   sparkleWave(ANIM_ID_SPARKLE_WAVE, matrix);
PulseWaveAnimation     pulseWave(ANIM_ID_PULSE_WAVE, matrix);
MatrixRainAnimation    matrixRain(ANIM_ID_MATRIX_RAIN, matrix);
NeonGridAnimation      neonGrid(ANIM_ID_NEON_GRID, matrix);
ScannerAnimation       scanner(ANIM_ID_SCANNER, matrix);
GalacticWarpAnimation  galacticWarp(ANIM_ID_GALACTIC_WARP, matrix);
FluoroLampAnimation    fluoro(ANIM_ID_FLUORO_LAMP, matrix);



void setup() {
	#if LOG_ENABLED
	Serial.begin(115200);
	// Do not block waiting for a host serial connection — allow the device to run
	// even when no serial console is attached.
	// If you need to wait for a USB CDC connection on native USB boards,
	// enable an explicit build-time flag and implement a conditional wait.
	LOG_PRINTLN("\n\n========================================");
	LOG_PRINTLN(" LedLine - LED Matrix Animation System");
	LOG_PRINTLN("========================================");
	LOG_PRINTLN("Starting LedLine...");
	#endif

	matrix.init();
	rotary.init();
		// Register animations with the manager (order = rotation order)
		animMgr.addAnimation(&stars);
		animMgr.addAnimation(&rainbow);
		animMgr.addAnimation(&plasma);
		animMgr.addAnimation(&sparkleWave);
		animMgr.addAnimation(&pulseWave);
		animMgr.addAnimation(&matrixRain);
		animMgr.addAnimation(&neonGrid);
		animMgr.addAnimation(&scanner);
		animMgr.addAnimation(&galacticWarp);
		animMgr.addAnimation(&fluoro);

	animMgr.init();
	rotary.attachListener(&app);
	
	app.begin();

	#if LOG_ENABLED
	LOG_PRINTLN("Setup complete. Debug mode ENABLED.");
	LOG_PRINTLN("Ready for hardware testing!");
	LOG_PRINTLN("========================================\n");
	#endif
}

void loop() {
	// update app (polls rotary, renders animations / handles poweroff)
	app.update();
}
