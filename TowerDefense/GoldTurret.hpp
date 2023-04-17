#ifndef GOLDTURRET_HPP
#define GOLDTURRET_HPP
#include "Turret.hpp"

class GoldTurret : public Turret {
public:
	static const int Price;
	GoldTurret(float x, float y);
	void CreateBullet() override;
	void Draw();
};
#endif // GOLDTURRET_HPP