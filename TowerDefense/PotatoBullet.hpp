#ifndef POTATOBULLET_HPP
#define POTATOBULLET_HPP
#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
	struct Point;
}  // namespace Engine

class PotatoBullet : public Bullet {
public:
	explicit PotatoBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret* parent);
	void OnExplode(Enemy* enemy) override;
};
#endif // POTATOBULLET_HPP
#pragma once
