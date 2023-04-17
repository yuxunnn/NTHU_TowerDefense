#include <allegro5/base.h>
#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <string>

#include "AudioHelper.hpp"
#include "Group.hpp"
#include "GoldTurret.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Enemy.hpp"
#include "Turret.hpp"

const int GoldTurret::Price = 120;
GoldTurret::GoldTurret(float x, float y) :
	Turret("play/turret-4.png", x, y, 20, Price, 0, 1) {
	Anchor.y += 8.0f / GetBitmapHeight();
	type = 1;
	BombRadius = 250;
}

void GoldTurret::CreateBullet() {

}