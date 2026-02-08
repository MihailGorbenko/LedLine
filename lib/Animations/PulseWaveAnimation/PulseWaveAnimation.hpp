#ifndef PULSE_WAVE_ANIMATION_HPP
#define PULSE_WAVE_ANIMATION_HPP

#include <Arduino.h>
#include "../../Animation/Animation.hpp"

#ifndef PULSEWAVE_DEFAULT_HUE
#define PULSEWAVE_DEFAULT_HUE 200  // cyan
#endif
#ifndef PULSEWAVE_DEFAULT_SAT
#define PULSEWAVE_DEFAULT_SAT 255
#endif
#ifndef PULSEWAVE_DEFAULT_VAL
#define PULSEWAVE_DEFAULT_VAL ANIMATION_DEFAULT_VAL
#endif

// Readable name macro (can be overridden before include)
#ifndef PULSEWAVE_ANIMATION_NAME
#define PULSEWAVE_ANIMATION_NAME "PulseWave"
#endif

class PulseWaveAnimation : public AnimationBase {
public:
	explicit PulseWaveAnimation(uint16_t id, LedMatrix& m);
	const char* getName() const override { return PULSEWAVE_ANIMATION_NAME; }
	void onActivate() override { AnimationBase::onActivate(); mw = matrix.getWidth(); mh = matrix.getHeight(); }
	void render() override;

private:
	uint8_t pulseRadius;  // 0..255
	int mw{0};
	int mh{0};
};

#endif // PULSE_WAVE_ANIMATION_HPP
