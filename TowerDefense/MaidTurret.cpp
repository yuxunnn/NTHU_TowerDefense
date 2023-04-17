#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "AudioHelper.hpp"
#include "FrenchFriesBullet.hpp"
#include "BurgerBullet.hpp"
#include "ChickenBullet.hpp"
#include "Group.hpp"
#include "MaidTurret.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Enemy.hpp"
#include "Sprite.hpp"
#include "PlayScene.hpp"

const int MaidTurret::Price = 200;

MaidTurret::MaidTurret(float x, float y) :
	Turret("play/turret-6.png", x, y, 20, Price, 0.2, 10) {
	// Move center downward, since we the turret head is slightly biased upward.
	Anchor.y += 8.0f / GetBitmapHeight();
	type = 7;
}
void MaidTurret::CreateBullet() {
	Engine::Point diff = Engine::Point(cos(Rotation /*- ALLEGRO_PI / 2*/), sin(Rotation /*- ALLEGRO_PI / 2*/));
	Engine::Point diff2 = Engine::Point(cos(Rotation /*- ALLEGRO_PI / 2*/), sin(Rotation/* - ALLEGRO_PI / 2*/) + 0.2);
	Engine::Point diff3 = Engine::Point(cos(Rotation /*- ALLEGRO_PI / 2*/), sin(Rotation /*- ALLEGRO_PI / 2*/) - 0.2);
	float rotation = ALLEGRO_PI / 2;
	Engine::Point normalized = diff.Normalize();
	Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
	// Change bullet position to the front of the gun barrel.
	getPlayScene()->BulletGroup->AddNewObject(new FrenchFriesBullet(Position + normalized * 36 - normal * 6, diff, rotation, this));
	getPlayScene()->BulletGroup->AddNewObject(new BurgerBullet(Position + normalized * 36 - normal * 6, diff2, rotation, this));
	getPlayScene()->BulletGroup->AddNewObject(new ChickenBullet(Position + normalized * 36 - normal * 6, diff3, rotation, this));
	AudioHelper::PlayAudio("gun.wav");
}
void MaidTurret::Update(float deltaTime) {
	Sprite::Update(deltaTime);
	PlayScene* scene = getPlayScene();
	if (!Enabled)
		return;
	if (Target) {
		if (Target->Position.x < Position.x) {
			Target->lockedTurrets.erase(lockedTurretIterator);
			Target = nullptr;
			lockedTurretIterator = std::list<Turret*>::iterator();
		}
	}
	if (!Target) {
		// Lock first seen target.
		// Can be improved by Spatial Hash, Quad Tree, ...
		// However simply loop through all enemies is enough for this program.
		int ty = static_cast<int>(floor(Position.y / PlayScene::BlockSize));
		int ey;
		for (auto& it : scene->EnemyGroup->GetObjects()) {
			ey = static_cast<int>(floor(it->Position.y / PlayScene::BlockSize));
			if (it->Position.x > Position.x && ey >= ty - 1 && ey <= ty + 1) {
				Target = dynamic_cast<Enemy*>(it);
				Target->lockedTurrets.push_back(this);
				lockedTurretIterator = std::prev(Target->lockedTurrets.end());
				break;
			}
		}
	}
	if (Target) {
		Engine::Point originRotation = Engine::Point(cos(Rotation /*- ALLEGRO_PI / 2*/), sin(Rotation /*- ALLEGRO_PI / 2*/));
		Engine::Point targetRotation = (Target->Position - Position).Normalize();
		float maxRotateRadian = rotateRadian * deltaTime;
		float cosTheta = originRotation.Dot(targetRotation);
		// Might have floating-point precision error.
		if (cosTheta > 1) cosTheta = 1;
		else if (cosTheta < -1) cosTheta = -1;
		float radian = acos(cosTheta);
		Engine::Point rotation;
		if (abs(radian) <= maxRotateRadian)
			rotation = targetRotation;
		else
			rotation = ((abs(radian) - maxRotateRadian) * originRotation + maxRotateRadian * targetRotation) / radian;
		// Add 90 degrees (PI/2 radian), since we assume the image is oriented upward.
		Rotation = atan2(rotation.y, rotation.x) /*+ ALLEGRO_PI / 2*/;
		// Shoot reload.
		reload -= deltaTime;
		if (reload <= 0) {
			// shoot.
			reload = coolDown;
			CreateBullet();
		}
	}
}
