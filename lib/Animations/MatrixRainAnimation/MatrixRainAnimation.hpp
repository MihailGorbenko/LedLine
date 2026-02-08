#ifndef MATRIX_RAIN_ANIMATION_HPP
#define MATRIX_RAIN_ANIMATION_HPP

#include <Arduino.h>
#include "../../Animation/Animation.hpp"
#include "../../LedMatrix/LedMatrix.hpp"

#ifndef MATRIXRAIN_DEFAULT_HUE
#define MATRIXRAIN_DEFAULT_HUE 96  // green (classic Matrix style)
#endif
#ifndef MATRIXRAIN_DEFAULT_SAT
#define MATRIXRAIN_DEFAULT_SAT 255
#endif
#ifndef MATRIXRAIN_DEFAULT_VAL
#define MATRIXRAIN_DEFAULT_VAL ANIMATION_DEFAULT_VAL
#endif

// Readable name macro (can be overridden before include)
#ifndef MATRIXRAIN_ANIMATION_NAME
#define MATRIXRAIN_ANIMATION_NAME "MatrixRain"
#endif

class MatrixRainAnimation : public AnimationBase {
public:
	explicit MatrixRainAnimation(uint16_t id, LedMatrix& m);
	const char* getName() const override { return MATRIXRAIN_ANIMATION_NAME; }
	void onActivate() override;
	void render() override;

private:
	static const int MAX_W = MATRIX_WIDTH;
	uint8_t cols[MAX_W];  // per-column rain position
	uint8_t intensity[MAX_W];  // per-column brightness
	int mw{0};
	int mh{0};
};

#endif // MATRIX_RAIN_ANIMATION_HPP
