#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "AudioHelper.hpp"
#include "PotatoBullet.hpp"
#include "Group.hpp"
#include "RedHatTurret.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Enemy.hpp"

const int RedHatTurret::Price = 100;

RedHatTurret::RedHatTurret(float x, float y) :
	Turret("play/turret-3.png", x, y, 20, Price, 0.35, 10) {
	// Move center downward, since we the turret head is slightly biased upward.
	Anchor.y += 8.0f / GetBitmapHeight();
	type = 6;
}

void RedHatTurret::CreateBullet() {
	Engine::Point diff = Engine::Point(1, 0);
	float rotation = ALLEGRO_PI / 2;
	getPlayScene()->BulletGroup->AddNewObject(new PotatoBullet(Position, diff, rotation, this));
	AudioHelper::PlayAudio("gun.wav");
}
