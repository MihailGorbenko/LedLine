#ifndef POWEROFF_ANIMATION_HPP
#define POWEROFF_ANIMATION_HPP

#include <Arduino.h>
#include "../../Animation/OverlayAnimation.hpp"

// default color (можно переопределить в проекте)
#ifndef POWEROFF_DEFAULT_HUE
#define POWEROFF_DEFAULT_HUE 85   // зелёный
#endif
#ifndef POWEROFF_DEFAULT_SAT
#define POWEROFF_DEFAULT_SAT 255
#endif
#ifndef POWEROFF_DEFAULT_VAL
#define POWEROFF_DEFAULT_VAL 255
#endif

class PowerOffAnimation : public OverlayAnimation {
public:
    explicit PowerOffAnimation(LedMatrix& m)
        : OverlayAnimation(m, POWEROFF_DEFAULT_HUE, POWEROFF_DEFAULT_SAT, POWEROFF_DEFAULT_VAL) {}

    void render() override;
};

#endif // POWEROFF_ANIMATION_HPP

