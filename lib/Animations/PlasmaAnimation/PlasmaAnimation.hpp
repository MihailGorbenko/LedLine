#ifndef PLASMA_ANIMATION_HPP
#define PLASMA_ANIMATION_HPP

#include <Arduino.h>
#include "../../Animation/Animation.hpp"
#include "../../LedMatrix/LedMatrix.hpp"

#ifndef PLASMA_DEFAULT_HUE
#define PLASMA_DEFAULT_HUE 160
#endif
#ifndef PLASMA_DEFAULT_SAT
#define PLASMA_DEFAULT_SAT 255
#endif
#ifndef PLASMA_DEFAULT_VAL
#define PLASMA_DEFAULT_VAL ANIMATION_DEFAULT_VAL
#endif

// Readable name macro (can be overridden before include)
#ifndef PLASMA_ANIMATION_NAME
#define PLASMA_ANIMATION_NAME "Plasma"
#endif

class PlasmaAnimation : public AnimationBase {
public:
	explicit PlasmaAnimation(uint16_t id, LedMatrix& m);
	const char* getName() const override { return PLASMA_ANIMATION_NAME; }
	void onActivate() override { AnimationBase::onActivate(); mw = matrix.getWidth(); mh = matrix.getHeight(); }
	void render() override;
private:
	int mw{0};
	int mh{0};
};

#endif // PLASMA_ANIMATION_HPP
