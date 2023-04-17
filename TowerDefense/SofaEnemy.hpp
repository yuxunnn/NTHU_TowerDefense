#ifndef SOFAENEMY_HPP
#define SOFAENEMY_HPP
#include "Enemy.hpp"

class SofaEnemy : public Enemy {
public:
	SofaEnemy(int x, int y);
	void CreateBullet();
};
#endif // SOFAENEMY_HPP
