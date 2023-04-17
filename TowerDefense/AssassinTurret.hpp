#ifndef ASSASSINTURRET_HPP
#define ASSASSINTURRET_HPP
#include "Turret.hpp"

class AssassinTurret : public Turret {
public:
	static const int Price;
	AssassinTurret(float x, float y);
	void CreateBullet() override;
	void Update(float deltaTime) override;
	int side = 0;
	Engine::Image* cross;
};
#endif // ASSASSINTURRET_HPP
