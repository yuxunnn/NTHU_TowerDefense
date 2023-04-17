#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "AudioHelper.hpp"
#include "IceCreamBullet.hpp"
#include "Group.hpp"
#include "TankTurret.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Enemy.hpp"

const int TankTurret::Price = 200;
TankTurret::TankTurret(float x, float y) :
	Turret("play/turret-5.png", x, y, 20, Price, 0.5, 100) {
	// Move center downward, since we the turret head is slightly biased upward.
	Anchor.y += 8.0f / GetBitmapHeight();
	type = 4;
}
void TankTurret::CreateBullet() {
	//Engine::Point diff = Engine::Point(1, 0);
	//float rotation = ALLEGRO_PI / 2;
	//getPlayScene()->BulletGroup->AddNewObject(new IceCreamBullet(Position, diff, rotation, this));
	//AudioHelper::PlayAudio("gun.wav");
}
