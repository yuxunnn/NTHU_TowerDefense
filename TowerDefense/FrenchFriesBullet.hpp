#ifndef FRENCHFRIESBULLET_HPP
#define FRENCHFRIESBULLET_HPP
#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
	struct Point;
}  // namespace Engine

class FrenchFriesBullet : public Bullet {
public:
	explicit FrenchFriesBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret* parent);
	void OnExplode(Enemy* enemy) override;
};
#endif // FRENCHFRIESBULLET_HPP
