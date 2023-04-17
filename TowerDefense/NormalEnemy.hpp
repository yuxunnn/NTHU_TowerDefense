#ifndef NORMALENEMY_HPP
#define NORMALENEMY_HPP
#include "Enemy.hpp"

class NormalEnemy : public Enemy {
public:
	NormalEnemy(int x, int y);
	void CreateBullet();
};
#endif // NORMALENEMY_HPP
