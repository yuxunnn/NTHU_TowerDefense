#include "Bullet.hpp"
#include "Collider.hpp"
#include "Enemy.hpp"
#include "EnemyBullet.hpp"
#include "GameEngine.hpp"
#include "Group.hpp"
#include "IObject.hpp"
#include "IScene.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Sprite.hpp"
#include "AudioHelper.hpp"

PlayScene* Bullet::getPlayScene() {
	return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
void Bullet::OnExplode(Enemy* enemy) {
}
Bullet::Bullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Turret* parent) :
	Sprite(img, position.x, position.y), speed(speed), damage(damage), parent(parent) {
	Velocity = forwardDirection * speed;
	Rotation = rotation;
	CollisionRadius = 4;
}
void Bullet::Update(float deltaTime) {
	Sprite::Update(deltaTime);
	PlayScene* scene = getPlayScene();
	// Can be improved by Spatial Hash, Quad Tree, ...
	// However simply loop through all enemies is enough for this program.
	for (auto& it : scene->EnemyGroup->GetObjects()) {
		Enemy* enemy = dynamic_cast<Enemy*>(it);
		if (!enemy->Visible)
			continue;
		if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, enemy->Position, enemy->CollisionRadius)) {
			OnExplode(enemy);
			if (freeze == 1) {
				if (enemy->speed >= 10) enemy->speed -= 50;
				else enemy->speed = 0;
				enemy->Velocity = Engine::Point(enemy->speed, 0);
				AudioHelper::PlayAudio("ice.wav");
			}
			enemy->Hit(damage);
			getPlayScene()->BulletGroup->RemoveObject(objectIterator);
			return;
		}
	}
	//for (auto& it : scene->EnemyBulletGroup->GetObjects()) {
	//	EnemyBullet* enemyBullet = dynamic_cast<EnemyBullet*>(it);
	//	if (!enemyBullet->Visible)
	//		continue;
	//	if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, enemyBullet->Position, enemyBullet->CollisionRadius)) {
	//		//OnExplode();
	//		getPlayScene()->EnemyBulletGroup->RemoveObject(enemyBullet->GetObjectIterator());
	//		getPlayScene()->BulletGroup->RemoveObject(objectIterator);
	//		return;
	//	}
	//}
	// Check if out of boundary.
	if (!Engine::Collider::IsRectOverlap(Position - Size / 2, Position + Size / 2, Engine::Point(0, 0), PlayScene::GetClientSize()))
		getPlayScene()->BulletGroup->RemoveObject(objectIterator);
}
