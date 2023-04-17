#ifndef UNKNOWNENEMY_HPP
#define UNKNOWNENEMY_HPP
#include "Enemy.hpp"

class UnknownEnemy : public Enemy {
public:
	UnknownEnemy(int x, int y);
	void CreateBullet();
};
#endif // UNKNOWNENEMY_HPP
