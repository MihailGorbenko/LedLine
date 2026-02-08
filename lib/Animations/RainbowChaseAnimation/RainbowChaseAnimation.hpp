#ifndef RAINBOW_CHASE_ANIMATION_HPP
#define RAINBOW_CHASE_ANIMATION_HPP

#include <Arduino.h>
#include "../../Animation/Animation.hpp"
#include "../../LedMatrix/LedMatrix.hpp"

// Default configuration (can be overridden at compile time)
#ifndef RAINBOWCHASE_DEFAULT_HUE
#define RAINBOWCHASE_DEFAULT_HUE 0
#endif
#ifndef RAINBOWCHASE_DEFAULT_SAT
#define RAINBOWCHASE_DEFAULT_SAT 255
#endif
#ifndef RAINBOWCHASE_DEFAULT_VAL
#define RAINBOWCHASE_DEFAULT_VAL ANIMATION_DEFAULT_VAL
#endif

// Readable name macro (can be overridden before include)
#ifndef RAINBOWCHASE_ANIMATION_NAME
#define RAINBOWCHASE_ANIMATION_NAME "RainbowChase"
#endif

class RainbowChaseAnimation : public AnimationBase {
public:
	explicit RainbowChaseAnimation(uint16_t id, LedMatrix& m);
	const char* getName() const override { return RAINBOWCHASE_ANIMATION_NAME; }
	void onActivate() override { AnimationBase::onActivate(); mw = matrix.getWidth(); mh = matrix.getHeight(); }
	void render() override;
private:
	int mw{0};
	int mh{0};
};

#endif // RAINBOW_CHASE_ANIMATION_HPP
