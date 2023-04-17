#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "AudioHelper.hpp"
#include "IceCreamBullet.hpp"
#include "Group.hpp"
#include "Bomb.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Enemy.hpp"

const int Bomb::Price = 0;
Bomb::Bomb(float x, float y) :
	Turret("play/bomb.png", x, y, 20, Price, 0, 1) {
	type = 2;
	Anchor.y += 8.0f / GetBitmapHeight();
}
void Bomb::CreateBullet() {
	
}
