#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "AudioHelper.hpp"
#include "IceCreamBullet.hpp"
#include "Group.hpp"
#include "AssassinTurret.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Enemy.hpp"
#include "CrossEffect.hpp"

const int AssassinTurret::Price = 220;
AssassinTurret::AssassinTurret(float x, float y) :
	Turret("play/turret-7.png", x, y, 20, Price, 0.5, 20) {
	// Move center downward, since we the turret head is slightly biased upward.
	Anchor.y += 8.0f / GetBitmapHeight();
	type = 3;
}
void AssassinTurret::CreateBullet() {
	
}
void AssassinTurret::Update(float deltaTime) {

	//Sprite::Update(deltaTime);
	PlayScene* scene = getPlayScene();
	if (!Enabled)
		return;
	/*if (Target) {
		if (Target->Position.x < Position.x) {
			Target->lockedTurrets.erase(lockedTurretIterator);
			Target = nullptr;
			lockedTurretIterator = std::list<Turret*>::iterator();
		}
	}*/
	if (!Target) {
		// Lock first seen target.
		// Can be improved by Spatial Hash, Quad Tree, ...
		// However simply loop through all enemies is enough for this program.
		/*int ty = static_cast<int>(floor(Position.y / PlayScene::BlockSize));
		int ey;*/
		for (auto& it : scene->EnemyGroup->GetObjects()) {
			//ey = static_cast<int>(floor(it->Position.y / PlayScene::BlockSize));

			Target = dynamic_cast<Enemy*>(it);
			Target->lockedTurrets.push_back(this);
			lockedTurretIterator = std::prev(Target->lockedTurrets.end());

			/*Position.x = Target->Position.x - 50;
			Position.y = Target->Position.y;*/

			break;
		}
	}
	if (Target) {
		//Engine::Point originRotation = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
		//Engine::Point targetRotation = (Target->Position - Position).Normalize();
		//float maxRotateRadian = rotateRadian * deltaTime;
		//float cosTheta = originRotation.Dot(targetRotation);
		//// Might have floating-point precision error.
		//if (cosTheta > 1) cosTheta = 1;
		//else if (cosTheta < -1) cosTheta = -1;
		//float radian = acos(cosTheta);
		//Engine::Point rotation;
		//if (abs(radian) <= maxRotateRadian)
		//	rotation = targetRotation;
		//else
		//	rotation = ((abs(radian) - maxRotateRadian) * originRotation + maxRotateRadian * targetRotation) / radian;
		//// Add 90 degrees (PI/2 radian), since we assume the image is oriented upward.
		//Rotation = atan2(rotation.y, rotation.x) + ALLEGRO_PI / 2;
		// Shoot reload.
		reload -= deltaTime;
		if (reload <= 0) {
			side = !side;
			// shoot.
			reload = coolDown;
			if (side == 0) {
				Position.x = Target->Position.x - 50;
				Position.y = Target->Position.y;
			}
			else {
				Position.x = Target->Position.x + 50;
				Position.y = Target->Position.y;
			}
			// Attack
			getPlayScene()->EffectGroup->AddNewObject(new CrossEffect(Target->Position.x, Target->Position.y));
			AudioHelper::PlayAudio("knife.mp3");
			Target->Hit(4);
		}
	}
}