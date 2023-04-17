#ifndef FATENEMY_HPP
#define FATENEMY_HPP
#include "Enemy.hpp"

class FatEnemy : public Enemy {
public:
	FatEnemy(int x, int y);
	void CreateBullet();
};
#endif // FATENEMY_HPP
