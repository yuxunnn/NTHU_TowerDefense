#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <list>
#include <vector>
#include <string>

#include "Point.hpp"
#include "Sprite.hpp"

class Bullet;
class PlayScene;
class Turret;

class Enemy : public Engine::Sprite {
protected:
	Engine::Point target;
	int money;
	float coolDown;

	float reload = 0;

	PlayScene* getPlayScene();
	virtual void OnExplode();

	virtual void CreateBullet() = 0;

public:
	float speed;
	bool stop = false;

	float hp;
	float reachEndTime;

	std::list<Turret*> lockedTurrets;
	std::list<Bullet*> lockedBullets;

	Enemy(std::string img, float x, float y, float radius, float speed, float hp, int money, float cooldown);
 	void Hit(float damage);
	void Update(float deltaTime) override;
	void Draw() const override;
};
#endif // ENEMY_HPP
