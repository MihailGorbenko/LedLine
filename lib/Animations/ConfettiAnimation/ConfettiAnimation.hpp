#ifndef CONFETTI_ANIMATION_HPP
#define CONFETTI_ANIMATION_HPP

#include <Arduino.h>
#include "../../Animation/Animation.hpp"

#ifndef CONFETTI_DEFAULT_HUE
#define CONFETTI_DEFAULT_HUE 0
#endif
#ifndef CONFETTI_DEFAULT_SAT
#define CONFETTI_DEFAULT_SAT 255
#endif
#ifndef CONFETTI_DEFAULT_VAL
#define CONFETTI_DEFAULT_VAL 80
#endif

class ConfettiAnimation : public AnimationBase {
public:
	explicit ConfettiAnimation(LedMatrix& m);
	void setColorHSV(uint8_t h, uint8_t s, uint8_t v) override;
	void render() override;

private:
	static const int MAX_W = MATRIX_WIDTH;
	static const int MAX_H = MATRIX_HEIGHT;
	static const int MAX_PIX = MAX_W * MAX_H;

	uint8_t vbuf[MAX_PIX];
	uint8_t hbuf[MAX_PIX];
};

#endif // CONFETTI_ANIMATION_HPP
