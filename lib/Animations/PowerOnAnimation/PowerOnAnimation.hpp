#ifndef POWERON_ANIMATION_HPP
#define POWERON_ANIMATION_HPP

#include <Arduino.h>
#include "../../Animation/OverlayAnimation.hpp"

#ifndef POWERON_DEFAULT_HUE
#define POWERON_DEFAULT_HUE 85
#endif
#ifndef POWERON_DEFAULT_SAT
#define POWERON_DEFAULT_SAT 255
#endif
#ifndef POWERON_DEFAULT_VAL
#define POWERON_DEFAULT_VAL 255
#endif

class PowerOnAnimation : public OverlayAnimation {
public:
    explicit PowerOnAnimation(LedMatrix& m)
        : OverlayAnimation(m, POWERON_DEFAULT_HUE, POWERON_DEFAULT_SAT, POWERON_DEFAULT_VAL) {}

    void render() override;
};

#endif // POWERON_ANIMATION_HPP
