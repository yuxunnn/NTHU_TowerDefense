#include <cmath>
#include <string>

#include "CrossEffect.hpp"
#include "GameEngine.hpp"
#include "Group.hpp"
#include "IScene.hpp"
#include "PlayScene.hpp"
#include "Resources.hpp"

PlayScene* CrossEffect::getPlayScene() {
	return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
CrossEffect::CrossEffect(float x, float y) : Sprite("play/cross-1.png", x, y), timeTicks(0) {
	for (int i = 1; i <= 2; i++) {
		bmps.push_back(Engine::Resources::GetInstance().GetBitmap("play/cross-" + std::to_string(i) + ".png"));
	}
}
void CrossEffect::Update(float deltaTime) {
	timeTicks += deltaTime;
	if (timeTicks >= timeSpan) {
		getPlayScene()->EffectGroup->RemoveObject(objectIterator);
		return;
	}
	int phase = floor(timeTicks / timeSpan * bmps.size());
	bmp = bmps[phase];
	Sprite::Update(deltaTime);
}
