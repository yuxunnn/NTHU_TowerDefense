#ifndef TANKTURRET_HPP
#define TANKTURRET_HPP
#include "Turret.hpp"

class TankTurret : public Turret {
public:
	static const int Price;
	TankTurret(float x, float y);
	void CreateBullet() override;
};
#endif // TANKTURRET_HPP
