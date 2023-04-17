#ifndef CROSSEFFECT_HPP
#define CROSSEFFECT_HPP
#include <allegro5/bitmap.h>
#include <memory>
#include <vector>

#include "Sprite.hpp"

class PlayScene;

class CrossEffect : public Engine::Sprite {
protected:
	PlayScene* getPlayScene();
	float timeTicks;
	std::vector<std::shared_ptr<ALLEGRO_BITMAP>> bmps;
	float timeSpan = 0.3;
public:
	CrossEffect(float x, float y);
	void Update(float deltaTime) override;
};
#endif // CROSSEFFECT_HPP
