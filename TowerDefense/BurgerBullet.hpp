#ifndef BURGERBULLET_HPP
#define BURGERBULLET_HPP
#include "Bullet.hpp"

class Enemy;
class Turret;

namespace Engine {
	struct Point;
}  // namespace Engine

class BurgerBullet : public Bullet {
public:
	explicit BurgerBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret* parent);
	void OnExplode(Enemy* enemy) override;
};
#endif // BURGERBULLET_HPP
