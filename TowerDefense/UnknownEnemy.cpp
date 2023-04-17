#include <string>

#include "UnknownEnemy.hpp"
#include "AudioHelper.hpp"
#include "PlayScene.hpp"
#include "Missile.hpp"


UnknownEnemy::UnknownEnemy(int x, int y) : Enemy("play/enemy-5.png", x, y, 10, 15, 50, 100, 2) {
}
void UnknownEnemy::CreateBullet() {
	Engine::Point diff = Engine::Point(-1, 0);
	float rotation = ALLEGRO_PI * 2;
	getPlayScene()->EnemyBulletGroup->AddNewObject(new Missile(Position, diff, rotation, this));
	AudioHelper::PlayAudio("missile.wav");
}