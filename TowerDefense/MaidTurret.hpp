#ifndef MAIDTURRET_HPP
#define MAIDTURRET_HPP
#include "Turret.hpp"

class MaidTurret : public Turret {
public:
	static const int Price;
	MaidTurret(float x, float y);
	void CreateBullet() override;
	void Update(float deltaTime) override;
};
#endif // MAIDTURRET_HPP
#pragma once
