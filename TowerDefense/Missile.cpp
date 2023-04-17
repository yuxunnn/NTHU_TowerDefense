#include <allegro5/base.h>
#include <random>
#include <string>

#include "DirtyEffect.hpp"
#include "Enemy.hpp"
#include "Missile.hpp"
#include "Group.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"

class Enemy;

Missile::Missile(Engine::Point position, Engine::Point forwardDirection, float rotation, Enemy* parent) :
	EnemyBullet("play/bullet-8.png", 800, 5, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent) {
}
void Missile::OnExplode(Turret* turret) {
	/*std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
	getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-3.png", dist(rng), turret->Position.x, turret->Position.y));*/
}