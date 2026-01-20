#ifndef GRADIENT_ANIMATION_HPP
#define GRADIENT_ANIMATION_HPP

#include <Arduino.h>
#include <vector>
#include "../Animation/Animation.hpp"

// default configuration (можно переопределить в проекте перед инклюдом)
#ifndef GRADIENT_DEFAULT_HUE
#define GRADIENT_DEFAULT_HUE 0
#endif
#ifndef GRADIENT_DEFAULT_SAT
#define GRADIENT_DEFAULT_SAT 255
#endif
#ifndef GRADIENT_DEFAULT_VAL
#define GRADIENT_DEFAULT_VAL 16
#endif
// размеры матрицы теперь получаются у LedMatrix через matrix->width()/height()

class GradientAnimation : public AnimationBase {
public:
	explicit GradientAnimation(LedMatrix& m);

	// Цвет динамический; setColorHSV — заглушка (обновляет поля, но render вычисляет цвета сам)
	void setColorHSV(uint8_t h, uint8_t s, uint8_t v) override;

	// Рисование кадра — вызывать с нужной частотой из контроллера
	void render() override;

	// Заглушки сохранения/загрузки цвета — цвет динамический, поэтому не сохраняем
	bool saveColor(const char* key);
	bool loadColor(const char* key);

private:
	// параметры градиента
	uint8_t hueSpan; // ширина градиента в hue-единицах
	bool reverse;

	// вспомогательные
	uint8_t computeHueForPos(unsigned long now, int pos, int total) const;
};

#endif // GRADIENT_ANIMATION_HPP
