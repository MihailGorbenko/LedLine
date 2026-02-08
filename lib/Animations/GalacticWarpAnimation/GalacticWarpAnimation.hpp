#ifndef GALACTIC_WARP_ANIMATION_HPP
#define GALACTIC_WARP_ANIMATION_HPP

#include <Arduino.h>
#include "../../Animation/Animation.hpp"
#include "../../LedMatrix/LedMatrix.hpp"

#ifndef GALACTICWARP_DEFAULT_HUE
#define GALACTICWARP_DEFAULT_HUE 240  // blue/purple
#endif
#ifndef GALACTICWARP_DEFAULT_SAT
#define GALACTICWARP_DEFAULT_SAT 220
#endif
#ifndef GALACTICWARP_DEFAULT_VAL
#define GALACTICWARP_DEFAULT_VAL ANIMATION_DEFAULT_VAL
#endif

// Readable name macro (can be overridden before include)
#ifndef GALACTICWARP_ANIMATION_NAME
#define GALACTICWARP_ANIMATION_NAME "GalacticWarp"
#endif

class GalacticWarpAnimation : public AnimationBase {
public:
	explicit GalacticWarpAnimation(uint16_t id, LedMatrix& m);
	const char* getName() const override { return GALACTICWARP_ANIMATION_NAME; }
	void onActivate() override { AnimationBase::onActivate(); mw = matrix.getWidth(); mh = matrix.getHeight(); }
	void render() override;
private:
	int mw{0};
	int mh{0};
};

#endif // GALACTIC_WARP_ANIMATION_HPP
