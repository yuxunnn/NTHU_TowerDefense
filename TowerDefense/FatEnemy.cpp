#include <string>

#include "FatEnemy.hpp"
#include "AudioHelper.hpp"
#include "PlayScene.hpp"
#include "BigFireBullet.hpp"


FatEnemy::FatEnemy(int x, int y) : Enemy("play/enemy-4.png", x, y, 10, 50, 50, 40, 0.8) {
}
void FatEnemy::CreateBullet() {
	Engine::Point diff = Engine::Point(-1, 0);
	float rotation = ALLEGRO_PI / 2;
	getPlayScene()->EnemyBulletGroup->AddNewObject(new BigFireBullet(Position, diff, rotation, this));
	AudioHelper::PlayAudio("gun.wav");
}