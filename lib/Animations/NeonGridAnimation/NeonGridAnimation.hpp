#ifndef NEON_GRID_ANIMATION_HPP
#define NEON_GRID_ANIMATION_HPP

#include <Arduino.h>
#include "../../Animation/Animation.hpp"

#ifndef NEONGRID_DEFAULT_HUE
#define NEONGRID_DEFAULT_HUE 160  // cyan
#endif
#ifndef NEONGRID_DEFAULT_SAT
#define NEONGRID_DEFAULT_SAT 255
#endif
#ifndef NEONGRID_DEFAULT_VAL
#define NEONGRID_DEFAULT_VAL ANIMATION_DEFAULT_VAL
#endif

// Readable name macro (can be overridden before include)
#ifndef NEONGRID_ANIMATION_NAME
#define NEONGRID_ANIMATION_NAME "NeonGrid"
#endif

class NeonGridAnimation : public AnimationBase {
public:
	explicit NeonGridAnimation(uint16_t id, LedMatrix& m);
	const char* getName() const override { return NEONGRID_ANIMATION_NAME; }
	void onActivate() override { AnimationBase::onActivate(); mw = matrix.getWidth(); mh = matrix.getHeight(); }
	void render() override;

private:
	static const int MAX_W = MATRIX_WIDTH;
	uint8_t gridPulse[MAX_W];  // per-column pulse state
	int mw{0};
	int mh{0};
};

#endif // NEON_GRID_ANIMATION_HPP
