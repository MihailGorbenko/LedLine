#ifndef FLUORO_LAMP_ANIMATION_HPP
#define FLUORO_LAMP_ANIMATION_HPP

#include <Arduino.h>
#include "../../Animation/Animation.hpp"

// Default: low saturation to look like white fluorescent light
#ifndef FLUOROLAMP_DEFAULT_HUE
#define FLUOROLAMP_DEFAULT_HUE 0
#endif
#ifndef FLUOROLAMP_DEFAULT_SAT
#define FLUOROLAMP_DEFAULT_SAT 10
#endif
#ifndef FLUOROLAMP_DEFAULT_VAL
#define FLUOROLAMP_DEFAULT_VAL ANIMATION_DEFAULT_VAL
#endif

// Readable name macro (can be overridden before include)
#ifndef FLUOROLAMP_ANIMATION_NAME
#define FLUOROLAMP_ANIMATION_NAME "FluoroLamp"
#endif

class FluoroLampAnimation : public AnimationBase {
public:
	explicit FluoroLampAnimation(uint16_t id, LedMatrix& m);
	const char* getName() const override { return FLUOROLAMP_ANIMATION_NAME; }
	void onActivate() override { AnimationBase::onActivate(); mw = matrix.getWidth(); mh = matrix.getHeight(); reset(); }
	void render() override;

	// Adjust warmup duration (ms) and flicker strength (0..255)
	void setWarmupMs(uint16_t ms);
	void setFlickerStrength(uint8_t amt);
	void reset();

private:
	// Timing and state
	unsigned long startedAt;
	uint16_t warmupMs;
	uint8_t flickerAmt; // how strong ongoing flicker is

	// Warmup blinking segment
	unsigned long blinkUntilMs;
	unsigned long nextBlinkAtMs;
	uint8_t blinkStartX;
	uint8_t blinkLen;

	void scheduleBlink(int w);
	int mw{0};
	int mh{0};
};

#endif // FLUORO_LAMP_ANIMATION_HPP
