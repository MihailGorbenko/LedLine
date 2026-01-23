#ifndef COSMIC_DUST_ANIMATION_HPP
#define COSMIC_DUST_ANIMATION_HPP

#include <Arduino.h>
#include "../../Animation/Animation.hpp"

#ifndef COSMICDUST_DEFAULT_HUE
#define COSMICDUST_DEFAULT_HUE 280  // purple
#endif
#ifndef COSMICDUST_DEFAULT_SAT
#define COSMICDUST_DEFAULT_SAT 200
#endif
#ifndef COSMICDUST_DEFAULT_VAL
#define COSMICDUST_DEFAULT_VAL 96
#endif

class CosmicDustAnimation : public AnimationBase {
public:
	explicit CosmicDustAnimation(LedMatrix& m);
	void setColorHSV(uint8_t h, uint8_t s, uint8_t v) override;
	void render() override;

private:
	struct Particle {
		uint8_t x, y;
		int8_t vx, vy;
		uint8_t brightness;
	};

	static const int PARTICLE_COUNT = 16;
	Particle particles[PARTICLE_COUNT];

	void updateParticles();
	void initParticle(int idx);
};

#endif // COSMIC_DUST_ANIMATION_HPP
