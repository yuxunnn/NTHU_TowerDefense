#ifndef MISSILE_HPP
#define MISSILE_HPP
#include "EnemyBullet.hpp"

class Enemy;
class Turret;

namespace Engine {
	struct Point;
}  // namespace Engine

class Missile : public EnemyBullet {
public:
	explicit Missile(Engine::Point position, Engine::Point forwardDirection, float rotation, Enemy* parent);
	void OnExplode(Turret* turret) override;
};
#endif // MISSILE_HPP
