#ifndef PROGRESS_ANIMATION_HPP
#define PROGRESS_ANIMATION_HPP

#include <Arduino.h>
#include "../../Animation/Animation.hpp"

#ifndef PROGRESS_DEFAULT_HUE
#define PROGRESS_DEFAULT_HUE 160 // blue
#endif
#ifndef PROGRESS_DEFAULT_SAT
#define PROGRESS_DEFAULT_SAT 255
#endif
#ifndef PROGRESS_DEFAULT_VAL
#define PROGRESS_DEFAULT_VAL ANIMATION_DEFAULT_VAL
#endif

class ProgressAnimation : public AnimationBase {
public:
	explicit ProgressAnimation(LedMatrix& m);
	void setColorHSV(uint8_t h, uint8_t s, uint8_t v) override;
	void render() override;

	// Set number of segments and which one is active (0-based)
	void setSegments(uint8_t total, uint8_t current);

private:
	uint8_t totalSeg;
	uint8_t currentSeg;
};

#endif // PROGRESS_ANIMATION_HPP
