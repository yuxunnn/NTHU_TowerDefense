#ifndef BIGFIREBULLET_HPP
#define BIGFIREBULLET_HPP
#include "EnemyBullet.hpp"

class Enemy;
class Turret;

namespace Engine {
	struct Point;
}  // namespace Engine

class BigFireBullet : public EnemyBullet {
public:
	explicit BigFireBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Enemy* parent);
	void OnExplode(Turret* turret) override;
};
#endif // BIGFIREBULLET_HPP
