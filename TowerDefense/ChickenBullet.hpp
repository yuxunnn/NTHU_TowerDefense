#ifndef CHICKENBULLET_HPP
#define CHICKENBULLET_HPP
#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
	struct Point;
}  // namespace Engine

class ChickenBullet : public Bullet {
public:
	explicit ChickenBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret* parent);
	void OnExplode(Enemy* enemy) override;
};
#endif // CHICKENBULLET_HPP
