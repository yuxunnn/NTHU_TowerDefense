#ifndef STRONGENEMY_HPP
#define STRONGENEMY_HPP
#include "Enemy.hpp"

class StrongEnemy : public Enemy {
public:
	StrongEnemy(int x, int y);
	void CreateBullet();
};
#endif // STRONGENEMY_HPP
