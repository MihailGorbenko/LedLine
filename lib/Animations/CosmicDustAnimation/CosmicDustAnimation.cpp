#include "CosmicDustAnimation.hpp"

CosmicDustAnimation::CosmicDustAnimation(LedMatrix& m)
	: AnimationBase(m, COSMICDUST_DEFAULT_HUE, COSMICDUST_DEFAULT_SAT, COSMICDUST_DEFAULT_VAL) {
	for (int i = 0; i < PARTICLE_COUNT; ++i) {
		initParticle(i);
	}
}

void CosmicDustAnimation::setColorHSV(uint8_t h, uint8_t s, uint8_t v) {
	// Cosmic dust: hue is base color, floating particles
	AnimationBase::setColorHSV(h, s, v);
}

void CosmicDustAnimation::initParticle(int idx) {
	particles[idx].x = (uint8_t)random(0, matrix ? matrix->width() : 8);
	particles[idx].y = (uint8_t)random(0, matrix ? matrix->height() : 2);
	particles[idx].vx = (int8_t)random(-2, 3);
	particles[idx].vy = (int8_t)random(-1, 2);
	particles[idx].brightness = (uint8_t)random(80, 255);
}

void CosmicDustAnimation::updateParticles() {
	int w = matrix ? matrix->width() : 8;
	int hgt = matrix ? matrix->height() : 2;

	for (int i = 0; i < PARTICLE_COUNT; ++i) {
		particles[i].x = (uint8_t)constrain((int)particles[i].x + particles[i].vx, 0, w - 1);
		particles[i].y = (uint8_t)constrain((int)particles[i].y + particles[i].vy, 0, hgt - 1);
		particles[i].brightness = scale8(particles[i].brightness, 245);  // slow decay

		if (particles[i].brightness < 20) {
			initParticle(i);
		}
	}
}

void CosmicDustAnimation::render() {
	if (!matrix) return;
	matrix->clear();

	int w = matrix->width();
	int hgt = matrix->height();
	if (w <= 0) w = 1;
	if (hgt <= 0) hgt = 1;

	updateParticles();

	// Draw particles with trail glow
	for (int i = 0; i < PARTICLE_COUNT; ++i) {
		uint8_t x = particles[i].x;
		uint8_t y = particles[i].y;
		uint8_t vOut = scale8(val, particles[i].brightness);
		uint8_t hOut = (uint8_t)(hue + (uint8_t)i * 16);  // rainbow effect across particles

		matrix->setPixelHSV(x, y, hOut, sat, vOut);

		// Add glow around particle
		if (x > 0) matrix->setPixelHSV(x - 1, y, hOut, sat, scale8(vOut, 128));
		if (x < w - 1) matrix->setPixelHSV(x + 1, y, hOut, sat, scale8(vOut, 128));
	}

	matrix->show();
}
