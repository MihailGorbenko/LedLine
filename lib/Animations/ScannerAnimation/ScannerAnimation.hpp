#ifndef SCANNER_ANIMATION_HPP
#define SCANNER_ANIMATION_HPP

#include <Arduino.h>
#include "../../Animation/Animation.hpp"
#include "../../LedMatrix/LedMatrix.hpp"

#ifndef SCANNER_DEFAULT_HUE
#define SCANNER_DEFAULT_HUE 150
#endif
#ifndef SCANNER_DEFAULT_SAT
#define SCANNER_DEFAULT_SAT 255
#endif
#ifndef SCANNER_DEFAULT_VAL
#define SCANNER_DEFAULT_VAL ANIMATION_DEFAULT_VAL
#endif

// Readable name macro (can be overridden before include)
#ifndef SCANNER_ANIMATION_NAME
#define SCANNER_ANIMATION_NAME "Scanner"
#endif

class ScannerAnimation : public AnimationBase {
public:
	explicit ScannerAnimation(uint16_t id, LedMatrix& m);
	const char* getName() const override { return SCANNER_ANIMATION_NAME; }
	void onActivate() override { AnimationBase::onActivate(); mw = matrix.getWidth(); mh = matrix.getHeight(); }
	void render() override;

private:
	uint8_t tail; // 0..255, bigger = longer/brighter tail
	int mw{0};
	int mh{0};
};

#endif // SCANNER_ANIMATION_HPP
