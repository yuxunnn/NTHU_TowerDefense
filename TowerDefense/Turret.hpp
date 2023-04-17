#ifndef TURRET_HPP
#define TURRET_HPP
#include <allegro5/base.h>
#include <list>
#include <string>

#include "Sprite.hpp"

class Enemy;
class PlayScene;
class EnemyBullet;

class Turret: public Engine::Sprite {
protected:
    int price;
    float coolDown;
    float reload = 0;
    float hp;
    float rotateRadian = 2 * ALLEGRO_PI;
    std::list<Turret*>::iterator lockedTurretIterator;
    PlayScene* getPlayScene();
    // Reference: Design Patterns - Factory Method.
    virtual void CreateBullet() = 0;

public:
    int type; // 0 : WBcellTurret
              // 1 : GoldTurret
              // 2 : Bomb
              // 3 : AssassinTurret
              // 4 : TankTurret
              // 5 : PlateletTurret
              // 6 : RedhatTurret
              // 7 : MaidTurret
    int BombRadius;
    int bombX, bombY;

    bool Enabled = true;
    bool Preview = false;
    Enemy* Target = nullptr;

    Turret(/*std::string imgBase,*/ std::string imgTurret, float x, float y, float radius, int price, float coolDown, float hp);
    void OnExplode();
    void Update(float deltaTime) override;
    void Draw() const override;
    void Hit(float hp);
	int GetPrice() const;
};



#endif // TURRET_HPP
