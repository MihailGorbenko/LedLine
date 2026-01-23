#pragma once
#include <Arduino.h>
#include <vector>
#include <Preferences.h>
#include "../RotaryEncoder/RotaryEncoder.hpp"
#include "../LedMatrix/LedMatrix.hpp"
#include "../Animation/Animation.hpp"
#include "../Animations/PowerOffAnimation/PowerOffAnimation.hpp"

// настройка FPS для обновления анимации (можно переопределить в проекте)
#ifndef APP_FPS
#define APP_FPS 30
#endif
// число шагов для яркости / цвета
#ifndef APP_STEPS
#define APP_STEPS 20
#endif
// длительность удержания для выключения (мс)
#ifndef APP_POWEROFF_HOLD_MS
#define APP_POWEROFF_HOLD_MS 2000
#endif

class AppController : public RotaryEncoder::IEncoderListener {
public:
	explicit AppController(LedMatrix& m, RotaryEncoder& enc);

	// добавить анимацию (в контроллере хранится указатель, владелец остаётся у вызывающего)
	void addAnimation(AnimationBase* a);

	// инициализация (вызвать в setup)
	void begin();

	// главный update — вызывать часто в loop()
	void update();

	// реализация интерфейса RotaryEncoder::IEncoderListener
	void onEvent(RotaryEncoder::Event ev, int value) override;

	// сохранение/загрузка состояния (NVS)
	bool saveState();
	bool loadState();

private:
	enum Mode { MODE_SELECT_ANIM = 0, MODE_BRIGHTNESS = 1, MODE_COLOR = 2, MODE_POWEROFF = 3 };

	LedMatrix* matrix;
	RotaryEncoder* encoder;
	std::vector<AnimationBase*> animations;
	int currentIndex;

	// режимы/состояние
	Mode mode;
	bool powered;
	bool powered_off_shown;  // флаг для предотвращения повторной очистки

	// яркость в шагах (0..APP_STEPS-1)
	int brightStep;
	// цветовой шаг (0..APP_STEPS-1) используется как дельта hue
	int colorStep;

	// poweroff handling
	bool btnDown;
	unsigned long btnPressedMillis;
	PowerOffAnimation powerOffAnim;

	// fps control
	unsigned long lastFrameMillis;
	unsigned long frameIntervalMs;

	// Вспомог.
	void applyMasterBrightness(); // применить яркость к матрице/анимации
};
