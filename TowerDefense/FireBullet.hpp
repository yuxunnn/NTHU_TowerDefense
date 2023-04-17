#ifndef FIREBULLET_HPP
#define FIREBULLET_HPP
#include "EnemyBullet.hpp"

class Enemy;
class Turret;

namespace Engine {
	struct Point;
}  // namespace Engine

class FireBullet : public EnemyBullet {
public:
	explicit FireBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Enemy* parent);
	void OnExplode(Turret* turret) override;
};
#endif // FIREBULLET_HPP
