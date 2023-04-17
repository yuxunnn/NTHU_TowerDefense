#ifndef REDHATTURRET_HPP
#define REDHATTURRET_HPP
#include "Turret.hpp"

class RedHatTurret : public Turret {
public:
	static const int Price;
	RedHatTurret(float x, float y);
	void CreateBullet() override;
};
#endif // REDHATTURRET_HPP
#pragma once
