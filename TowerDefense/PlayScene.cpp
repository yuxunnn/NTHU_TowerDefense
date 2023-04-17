#include <allegro5/allegro.h>
#include <algorithm>
#include <random>
#include <cmath>
#include <fstream>
#include <functional>
#include <vector>
#include <queue>
#include <string>
#include <memory>
#include <numeric> 

#include "AudioHelper.hpp"
#include "DirtyEffect.hpp"
#include "Enemy.hpp"
#include "GameEngine.hpp"
#include "Group.hpp"
#include "IObject.hpp"
#include "Image.hpp"
#include "Label.hpp"
#include "NormalEnemy.hpp"
#include "PlateletTurret.hpp"
#include "WBCellTurret.hpp"
#include "RedHatTurret.hpp"
#include "GoldTurret.hpp"
#include "MaidTurret.hpp"
#include "TankTurret.hpp"
#include "AssassinTurret.hpp"
#include "Bomb.hpp"
#include "Plane.hpp"
#include "PlayScene.hpp"
#include "Resources.hpp"
#include "SofaEnemy.hpp"
#include "StrongEnemy.hpp"
#include "FatEnemy.hpp"
#include "UnknownEnemy.hpp"
#include "Sprite.hpp"
#include "Turret.hpp"
#include "TurretButton.hpp"
#include "LOG.hpp"


bool PlayScene::DebugMode = false;

const std::vector<Engine::Point> PlayScene::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
const int PlayScene::MapWidth = 12, PlayScene::MapHeight = 6;//50;//13;
const int PlayScene::BlockSize = 128;
const float PlayScene::DangerTime = 7.61;
const int PlayScene::SpawnGridPointx = 12;
const int PlayScene::EndGridPointx = -1;
// TODO 4 (1/3): Set a cheat sequence here.
const std::vector <int> PlayScene::code = { ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_ENTER };

Engine::Point PlayScene::GetClientSize() {
	return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void PlayScene::Initialize() {
	mapState.clear();
	keyStrokes.clear();
	ticks = 0;
	deathCountDown = -1;
	lives = 20;
	money = 180;
	SpeedMult = 1;
	KillCount = 0;
	laneNum = std::vector<int>(MapHeight);
	std::iota(laneNum.begin(), laneNum.end(), 0);
	// Add groups from bottom to top.
	AddNewObject(TileMapGroup = new Group());
	AddNewObject(GroundEffectGroup = new Group());
	AddNewObject(DebugIndicatorGroup = new Group());
	AddNewObject(TowerGroup = new Group());
	AddNewObject(EnemyGroup = new Group());
	AddNewObject(BulletGroup = new Group());
	AddNewObject(EnemyBulletGroup = new Group());
	AddNewObject(EffectGroup = new Group());
	// Should support buttons.
	AddNewControlObject(UIGroup = new Group());
	ReadMap();
	ReadEnemyWave();
	ConstructUI();
	imgTarget = new Engine::Image("play/target.png", 0, 0);
	imgTarget->Visible = false;
	preview = nullptr;
	UIGroup->AddNewObject(imgTarget);
	// Preload Lose Scene
	deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
	Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
	// Start BGM.
	bgmId = AudioHelper::PlayBGM("play.ogg");
}
void PlayScene::Terminate() {
	AudioHelper::StopBGM(bgmId);
	AudioHelper::StopSample(deathBGMInstance);
	deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
	// If we use deltaTime directly, then we might have Bullet-through-paper problem.
	// Reference: Bullet-Through-Paper
	if (SpeedMult == 0)
		deathCountDown = -1;
	else if (deathCountDown != -1)
		SpeedMult = 1;
	// Calculate danger zone.
	std::vector<float> reachEndTimes;
	for (auto& it : EnemyGroup->GetObjects()) {
		reachEndTimes.push_back(dynamic_cast<Enemy*>(it)->reachEndTime);
	}
	// Can use Heap / Priority-Queue instead. But since we won't have too many enemies, sorting is fast enough.
	std::sort(reachEndTimes.begin(), reachEndTimes.end());
	float newDeathCountDown = -1;
	int danger = lives;
	for (auto& it : reachEndTimes) {
		if (it <= DangerTime) {
			danger--;
			if (danger <= 0) {
				// Death Countdown
				float pos = DangerTime - it;
				if (it > deathCountDown) {
					// Restart Death Count Down BGM.
					AudioHelper::StopSample(deathBGMInstance);
					if (SpeedMult != 0)
						deathBGMInstance = AudioHelper::PlaySample("astronomia.ogg", false, AudioHelper::BGMVolume, pos);
				}
				float alpha = pos / DangerTime;
				alpha = std::max(0, std::min(255, static_cast<int>(alpha * alpha * 255)));
				dangerIndicator->Tint = al_map_rgba(255, 255, 255, alpha);
				newDeathCountDown = it;
				break;
			}
		}
	}
	deathCountDown = newDeathCountDown;
	if (SpeedMult == 0)
		AudioHelper::StopSample(deathBGMInstance);
	if (deathCountDown == -1 && lives > 0) {
		AudioHelper::StopSample(deathBGMInstance);
		dangerIndicator->Tint.a = 0;
	}
	if (SpeedMult == 0)
		deathCountDown = -1;
	for (int i = 0; i < SpeedMult; i++) {
	    IScene::Update(deltaTime);
		// Check if we should create new enemy.
		ticks += deltaTime;
		if (enemyWaveData.empty()) {
			if (EnemyGroup->GetObjects().empty()) {
				// Win.
				Engine::GameEngine::GetInstance().ChangeScene("win");
			}
			continue;
		}
		auto current = enemyWaveData.front();
		if (ticks < std::get<1>(current))//change
			continue;

		ticks -= std::get<1>(current);
		enemyWaveData.pop_front();
        std::random_device dev;
	    std::mt19937 rng(dev());
		std::shuffle(laneNum.begin(), laneNum.end(), rng);
        std::uniform_int_distribution<std::mt19937::result_type> dist(1, 3);
        Enemy* enemy;
        for(int j = 0 ; j < std::get<2>(current) ; j++){
            const Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPointx * BlockSize + BlockSize / 2 + dist(rng) * 15, laneNum[j] * BlockSize + BlockSize / 2);
            switch (std::get<0>(current)) {
				case 1:
					EnemyGroup->AddNewObject(enemy = new NormalEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
					break;
				case 2:
					EnemyGroup->AddNewObject(enemy = new SofaEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
					break;
				case 3:
					EnemyGroup->AddNewObject(enemy = new StrongEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
					break;
					// TODO 2 (7/8): You need to modify 'resources/enemy1.txt', or 'resources/enemy2.txt' to spawn the 4th enemy.
					//         The format is "[EnemyId] [TimeDelay] [LaneNum] [Repeat]".
					// TODO 2 (8/8): Enable the creation of the 4th enemy.
				case 4:
					EnemyGroup->AddNewObject(enemy = new FatEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
					break;
				case 5:
					EnemyGroup->AddNewObject(enemy = new UnknownEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
					break;
				default:
					continue;
			}
		    // Compensate the time lost.
		    enemy->Update(ticks);
		}
	}
	if (preview) {
		preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
		// To keep responding when paused.
		preview->Update(deltaTime);
	}
}
void PlayScene::Draw() const {
	IScene::Draw();
}
void PlayScene::OnMouseDown(int button, int mx, int my) {
	if ((button & 1) && !imgTarget->Visible && preview) {
		// Cancel turret construct.
		UIGroup->RemoveObject(preview->GetObjectIterator());
		preview = nullptr;
	}
	IScene::OnMouseDown(button, mx, my);
}
void PlayScene::OnMouseMove(int mx, int my) {
	IScene::OnMouseMove(mx, my);

	if (preview) Info(preview, mx, my);
	else UIInfo->Visible = false;
	
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) {
		imgTarget->Visible = false;
		return;
	}
	imgTarget->Visible = true;
	imgTarget->Position.x = x * BlockSize;
	imgTarget->Position.y = y * BlockSize;
}
void PlayScene::Info(Turret* preview, int mx, int my) {
	if(DebugMode) UIInfo->Visible = true;
	if(preview->type == 0) UIInfo->Text = std::string("Shoot Icecream");
	else if(preview->type == 1) UIInfo->Text = std::string("Explode");
	else if(preview->type == 2)	UIInfo->Text = std::string("Clear");
	else if(preview->type == 3)	UIInfo->Text = std::string("Assassin");
	else if(preview->type == 4)	UIInfo->Text = std::string("Tank and Earn money");
	else if(preview->type == 5) UIInfo->Text = std::string("Shoot Pocky");
	else if (preview->type == 6) UIInfo->Text = std::string("Freeze ememy");
	else if (preview->type == 7) UIInfo->Text = std::string("Triple shoot");
	UIInfo->Position.x = preview->Position.x - UIInfo->GetTextWidth() / 2;
	UIInfo->Position.y = preview->Position.y + preview->GetBitmapHeight() / 2;
}
void PlayScene::OnMouseUp(int button, int mx, int my) {
	IScene::OnMouseUp(button, mx, my);
	if (!imgTarget->Visible)
		return;
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	if (button & 1) {
		if (mapState[y][x] != TILE_OCCUPIED) {
			if (!preview)
				return;
			// Check if valid.
			if (!CheckSpaceValid(x, y) || preview->type == 2) {
				
				Engine::Sprite* sprite;
				GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
				sprite->Rotation = 0;
				return;
			}

			if(preview->type != 3) mapState[y][x] = TILE_OCCUPIED;
			
			// Purchase.
			EarnMoney(-preview->GetPrice());
			// Remove Preview.
			preview->GetObjectIterator()->first = false;
			UIGroup->RemoveObject(preview->GetObjectIterator());
			// Construct real turret.
			preview->Position.x = x * BlockSize + BlockSize / 2;
			preview->Position.y = y * BlockSize + BlockSize / 2;
			preview->Enabled = true;
			preview->Preview = false;
			preview->Tint = al_map_rgba(255, 255, 255, 255);
			TowerGroup->AddNewObject(preview);
			// To keep responding when paused.
			preview->Update(0);
			// Remove Preview.
			preview = nullptr;

			OnMouseMove(mx, my);
		}
		else if (preview->type == 2) {
			if (!preview)
				return;
			// Check if valid.
			/*if (!CheckSpaceValid(x, y)) {

				Engine::Sprite* sprite;
				GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
				sprite->Rotation = 0;
				return;
			}*/
			// Remove Preview.
			preview->GetObjectIterator()->first = false;
			UIGroup->RemoveObject(preview->GetObjectIterator());
			// Construct real turret.
			/*preview->Position.x = x * BlockSize + BlockSize / 2;
			preview->Position.y = y * BlockSize + BlockSize / 2;
			preview->Enabled = true;
			preview->Preview = false;
			preview->Tint = al_map_rgba(255, 255, 255, 255);
			TowerGroup->AddNewObject(preview);*/
			// To keep responding when paused.
			preview->Update(0);
			// Remove Preview.
			preview = nullptr;

			for (auto& it : this->TowerGroup->GetObjects()) {
				Turret* turret = dynamic_cast<Turret*>(it);
				if (!turret->Visible)
					continue;
				Engine::Point pnt;
				pnt.x = floor(turret->Position.x / BlockSize);
				pnt.y = floor(turret->Position.y / BlockSize);
				if (pnt.x < 0) pnt.x = 0;
				if (pnt.x >= MapWidth) pnt.x = MapWidth - 1;
				if (pnt.y < 0) pnt.y = 0;
				if (pnt.y >= MapHeight) pnt.y = MapHeight - 1;
				if (x == pnt.x && y == pnt.y) {
					turret->Hit(INFINITY);
					break;
				}
			}

			// mapState[y][x] = TILE_OCCUPIED;
			OnMouseMove(mx, my);
		}
	
	}
}
void PlayScene::OnKeyDown(int keyCode) {
	IScene::OnKeyDown(keyCode);
	if (keyCode == ALLEGRO_KEY_BACKSPACE)
		Engine::GameEngine::GetInstance().ChangeScene("stage-select");
	// TODO 4 (2/3): Set Tab as a code to active or de-active debug mode
	if (keyCode == ALLEGRO_KEY_TAB) {
		DebugMode = !DebugMode;
		if (!DebugMode) UIInfo->Visible = false;
		/*else UIInfo->Visible = true;*/
	}
	if(DebugMode) {

		keyStrokes.push_back(keyCode);
		if (keyStrokes.size() > code.size())
			keyStrokes.pop_front();
        // TODO 4 (3/3): Check whether the input sequence is correct
		if (keyStrokes.size() == code.size()) {
			auto i = keyStrokes.begin();
			int j;
			for (j=0; i != keyStrokes.end(); i++, j++) {
				if (code[j] != *i) break;
			}
			if (j == code.size()) {
				EffectGroup->AddNewObject(new Plane());
			}
		}
	}
	if (keyCode == ALLEGRO_KEY_Q) {
		// Hotkey for WBCellTurret.
		UIBtnClicked(0);
	}
	else if (keyCode == ALLEGRO_KEY_W) {
		// Hotkey for PlateletTurret.
		UIBtnClicked(1);
	}
	// TODO 2 (5/8): Make the E key to create the 3th turret.
	else if (keyCode == ALLEGRO_KEY_E)
		// Hotkey for RedHatTurret
		UIBtnClicked(2);
	else if (keyCode == ALLEGRO_KEY_R)
		// Hotkey for GoldTurret
		UIBtnClicked(3);
	else if (keyCode == ALLEGRO_KEY_A)
		// Hotkey for MaidTurret
		UIBtnClicked(4);
	else if (keyCode == ALLEGRO_KEY_S)
		// Hotkey for TankTurret
		UIBtnClicked(5);
	else if (keyCode == ALLEGRO_KEY_D)
		// Hotkey for AssassinTurret
		UIBtnClicked(6);
	else if (keyCode == ALLEGRO_KEY_F)
		// Hotkey for Bomb
		UIBtnClicked(7);

	else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
		// Hotkey for Speed up.
		SpeedMult = keyCode - ALLEGRO_KEY_0;
	}
}
void PlayScene::Hit() {
	if (lives > 1) {
		lives--;
		UILives->Text = std::string("Life ") + std::to_string(lives);
	}	
	else {
		// Free resources.
			/*delete TileMapGroup;
			delete GroundEffectGroup;
			delete DebugIndicatorGroup;
			delete TowerGroup;
			delete EnemyGroup;
			delete BulletGroup;
			delete EffectGroup;
			delete UIGroup;
			delete imgTarget;*/
		//lose
		Engine::GameEngine::GetInstance().ChangeScene("lose");
	}
}
int PlayScene::GetMoney() const {
	return money;
}
void PlayScene::EarnMoney(int money) {
	this->money += money;
	UIMoney->Text = std::string("$") + std::to_string(this->money);
	UIKills->Text = std::string("kill ") + std::to_string(KillCount);
}
void PlayScene::ReadMap() {
	std::string filename = std::string("resources/map") + std::to_string(MapId) + ".txt";
	// Read map file.
	char c;
	std::vector<bool> mapData;
	std::ifstream fin(filename);
	while (fin >> c) {
		switch (c) {
		case '0': mapData.push_back(false); break;
		case '1': mapData.push_back(true); break;
		case '\n':
		case '\r':
			if (static_cast<int>(mapData.size()) / MapWidth != 0)
				throw std::ios_base::failure("Map data is corrupted.");
			break;
		default: throw std::ios_base::failure("Map data is corrupted.");
		}
	}
	fin.close();
	// Validate map data.
	if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
		throw std::ios_base::failure("Map data is corrupted.");
	// Store map in 2d array.
	mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
	for (int i = 0; i < MapHeight; i++) {
		for (int j = 0; j < MapWidth; j++) {
			const int num = mapData[i * MapWidth + j];
			mapState[i][j] = num ? TILE_FLOOR : TILE_DIRT;
			if (num)
				TileMapGroup->AddNewObject(new Engine::Image("play/deep-pink.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
			else
				TileMapGroup->AddNewObject(new Engine::Image("play/pink.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
		}
	}
}
void PlayScene::ReadEnemyWave() {
	std::string filename = std::string("resources/enemy") + std::to_string(MapId) + ".txt";
	// Read enemy file.
	float type, wait, totallane,repeat; //change
	enemyWaveData.clear();
	std::ifstream fin(filename);
	while (fin >> type && fin >> wait && fin >> totallane && fin >> repeat) {
		for (int i = 0; i < repeat; i++)
			enemyWaveData.emplace_back(type, wait, totallane);
	}
	fin.close();
}
void PlayScene::ConstructUI() {
	// Background
	UIGroup->AddNewObject(new Engine::Image("play/sand.png", 0, 128*MapHeight, 1536, 128));
	// Text
	//UIGroup->AddNewObject(new Engine::Label(std::string("Stage ") + std::to_string(MapId), "pirulen.ttf", 32, 1294, 0));
	UIGroup->AddNewObject(UIMoney = new Engine::Label(std::string("$") + std::to_string(money), "pirulen.ttf", 24, 1294, 128*MapHeight));
	UIGroup->AddNewObject(UILives = new Engine::Label(std::string("Life ") + std::to_string(lives), "pirulen.ttf", 24, 1294, 128*MapHeight+30));
	UIGroup->AddNewObject(UIKills = new Engine::Label(std::string("Kill ") + std::to_string(KillCount), "pirulen.ttf", 24, 1294, 128 * MapHeight + 60));

    TurretButton* btn;
	// Button 1
	btn = new TurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/turret-1.png", 180, BlockSize*MapHeight, 0, 0, 0, 0)
		, 170, 128*MapHeight, WBCellTurret::Price);
	// Reference: Class Member Function Pointer and std::bind.
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 0));
	UIGroup->AddNewControlObject(btn);
	UIGroup->AddNewObject(new Engine::Label(std::string("$") + std::to_string(WBCellTurret::Price), "pirulen.ttf", 16, 170, 128 * MapHeight + 90, 100, 50, 200));
	UIGroup->AddNewObject(new Engine::Label(std::string("Q"), "pirulen.ttf", 16, 170, 128 * MapHeight, 200, 200, 0));
	// Button 2
	btn = new TurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/turret-2.png", 300, BlockSize*MapHeight, 0, 0, 0, 0)
		, 290, 128*MapHeight, PlateletTurret::Price);
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 1));
	UIGroup->AddNewControlObject(btn);
	UIGroup->AddNewObject(new Engine::Label(std::string("$") + std::to_string(PlateletTurret::Price), "pirulen.ttf", 16, 290, 128 * MapHeight + 90, 100, 50, 200));
	UIGroup->AddNewObject(new Engine::Label(std::string("W"), "pirulen.ttf", 16, 290, 128 * MapHeight, 200, 200, 0));
	// TODO 2 (3/8): Create a button to support constructing the 3th tower.
	// Button 3
	btn = new TurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/turret-3.png", 420, BlockSize * MapHeight, 0, 0, 0, 0)
		, 410, 128 * MapHeight, RedHatTurret::Price);
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 2));
	UIGroup->AddNewControlObject(btn);
	UIGroup->AddNewObject(new Engine::Label(std::string("$") + std::to_string(RedHatTurret::Price), "pirulen.ttf", 16, 410, 128 * MapHeight + 90, 100, 50, 200));
	UIGroup->AddNewObject(new Engine::Label(std::string("E"), "pirulen.ttf", 16, 410, 128 * MapHeight, 200, 200, 0));
	// Button 4
	btn = new TurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/turret-4.png", 540, BlockSize * MapHeight, 0, 0, 0, 0)
		, 530, 128 * MapHeight, GoldTurret::Price);
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 3));
	UIGroup->AddNewControlObject(btn);
	UIGroup->AddNewObject(new Engine::Label(std::string("$") + std::to_string(GoldTurret::Price), "pirulen.ttf", 16, 530, 128 * MapHeight + 90, 100, 50, 200));
	UIGroup->AddNewObject(new Engine::Label(std::string("R"), "pirulen.ttf", 16, 530, 128 * MapHeight, 200, 200, 0));
	// Button 5
	btn = new TurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/turret-6.png", 660, BlockSize * MapHeight, 0, 0, 0, 0)
		, 650, 128 * MapHeight, MaidTurret::Price);
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 4));
	UIGroup->AddNewControlObject(btn);
	UIGroup->AddNewObject(new Engine::Label(std::string("$") + std::to_string(MaidTurret::Price), "pirulen.ttf", 16, 650, 128 * MapHeight + 90, 100, 50, 200));
	UIGroup->AddNewObject(new Engine::Label(std::string("A"), "pirulen.ttf", 16, 650, 128 * MapHeight, 200, 200, 0));

	// Button 6
	btn = new TurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/turret-5.png", 780, BlockSize * MapHeight, 0, 0, 0, 0)
		, 770, 128 * MapHeight, TankTurret::Price);
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 5));
	UIGroup->AddNewControlObject(btn);
	UIGroup->AddNewObject(new Engine::Label(std::string("$") + std::to_string(TankTurret::Price), "pirulen.ttf", 16, 770, 128 * MapHeight + 90, 100, 50, 200));
	UIGroup->AddNewObject(new Engine::Label(std::string("S"), "pirulen.ttf", 16, 770, 128 * MapHeight, 200, 200, 0));

	// Button 7
	btn = new TurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/turret-7.png", 900, BlockSize * MapHeight, 0, 0, 0, 0)
		, 890, 128 * MapHeight, AssassinTurret::Price);
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 6));
	UIGroup->AddNewControlObject(btn);
	UIGroup->AddNewObject(new Engine::Label(std::string("$") + std::to_string(AssassinTurret::Price), "pirulen.ttf", 16, 890, 128 * MapHeight + 90, 100, 50, 200));
	UIGroup->AddNewObject(new Engine::Label(std::string("D"), "pirulen.ttf", 16, 890, 128 * MapHeight, 200, 200, 0));

	// Button 8
	btn = new TurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/bomb.png", 1020, BlockSize * MapHeight, 0, 0, 0, 0)
		, 1010, 128 * MapHeight, Bomb::Price);
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 7));
	UIGroup->AddNewControlObject(btn);
	UIGroup->AddNewObject(new Engine::Label(std::string("$") + std::to_string(Bomb::Price), "pirulen.ttf", 16, 1010, 128 * MapHeight + 90, 100, 50, 200));
	UIGroup->AddNewObject(new Engine::Label(std::string("F"), "pirulen.ttf", 16, 1010, 128 * MapHeight, 200, 200, 0));

	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int shift = 135 + 25;
	dangerIndicator = new Engine::Sprite("play/benjamin.png", w - shift, h - shift);
	dangerIndicator->Tint.a = 0;
	UIGroup->AddNewObject(dangerIndicator);

	UIGroup->AddNewObject(UIInfo = new Engine::Label(std::string("Shoot"), "pirulen.ttf", 14, 0, 0, 100, 110, 255));
	UIInfo->Visible = false;
}

void PlayScene::UIBtnClicked(int id) {
	if (preview) {
		UIGroup->RemoveObject(preview->GetObjectIterator());
		preview = nullptr;
	}
	if (id == 0 && money >= WBCellTurret::Price)
		preview = new WBCellTurret(0, 0);
	else if (id == 1 && money >= PlateletTurret::Price)
		preview = new PlateletTurret(0, 0);
	// TODO 2 (4/8): On callback, create the 3th tower.
	else if (id == 2 && money >= RedHatTurret::Price)
		preview = new RedHatTurret(0, 0);
	else if (id == 3 && money >= GoldTurret::Price)
		preview = new GoldTurret(0, 0);
	else if (id == 4 && money >= MaidTurret::Price)
		preview = new MaidTurret(0, 0);
	else if (id == 5 && money >= TankTurret::Price)
		preview = new TankTurret(0, 0);
	else if (id == 6 && money >= AssassinTurret::Price)
		preview = new AssassinTurret(0, 0);
	else if (id == 7 && money >= Bomb::Price) 
		preview = new Bomb(0, 0);

	if (!preview) {
		UIInfo->Visible = false;
		return;
	}
	preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
	preview->Tint = al_map_rgba(255, 255, 255, 200);
	preview->Enabled = false;
	preview->Preview = true;
	UIGroup->AddNewObject(preview);
	OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
}

bool PlayScene::CheckSpaceValid(int x, int y) {
	if (x < 0 || x >= MapWidth  || y < 0 || y >= MapHeight  )
		return false;
	for (auto& it : EnemyGroup->GetObjects()) {
		Engine::Point pnt;
		pnt.x = floor(it->Position.x / BlockSize);
		pnt.y = floor(it->Position.y / BlockSize);
		if (pnt.x < 0) pnt.x = 0;
		if (pnt.x >= MapWidth) pnt.x = MapWidth - 1;
		if (pnt.y < 0) pnt.y = 0;
		if (pnt.y >= MapHeight) pnt.y = MapHeight - 1;
		if (pnt.x == x && pnt.y == y){
			return false;
		}
	}
	return true;
}
