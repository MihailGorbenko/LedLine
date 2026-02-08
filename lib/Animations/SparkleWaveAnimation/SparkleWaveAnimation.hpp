#ifndef SPARKLE_WAVE_ANIMATION_HPP
#define SPARKLE_WAVE_ANIMATION_HPP

#include <Arduino.h>
#include "../../Animation/Animation.hpp"

#ifndef SPARKLEWAVE_DEFAULT_HUE
#define SPARKLEWAVE_DEFAULT_HUE 96
#endif
#ifndef SPARKLEWAVE_DEFAULT_SAT
#define SPARKLEWAVE_DEFAULT_SAT 255
#endif
#ifndef SPARKLEWAVE_DEFAULT_VAL
#define SPARKLEWAVE_DEFAULT_VAL ANIMATION_DEFAULT_VAL
#endif

// Readable name macro (can be overridden before include)
#ifndef SPARKLEWAVE_ANIMATION_NAME
#define SPARKLEWAVE_ANIMATION_NAME "SparkleWave"
#endif

class SparkleWaveAnimation : public AnimationBase {
public:
	explicit SparkleWaveAnimation(uint16_t id, LedMatrix& m);
	const char* getName() const override { return SPARKLEWAVE_ANIMATION_NAME; }
	void onActivate() override { AnimationBase::onActivate(); mw = matrix.getWidth(); mh = matrix.getHeight(); }
	void render() override;

private:
	uint8_t sparkleChance; // 0..255
	int mw{0};
	int mh{0};
};

#endif // SPARKLE_WAVE_ANIMATION_HPP
