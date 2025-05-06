#include "Level.h"

#include "Renderer.h"
#include "DDLevelLoad.h"
#include "gameobjectpool.h"
#include "logmanager.h"
#include "inputsystem.h"
#include "quadtree.h"
#include "soundsystem.h"

#include "imgui/imgui.h"

#include "HUDParser.h"
#include "TileParser.h"

#include "water.h"
#include "player.h"
#include "weapon.h"
#include "bullet.h"

Level::Level() : m_playerSize(48.0f), m_soundSystem(0), m_playerPrevPosition(0, 0), m_playerPosition(0, 0), m_currentPlayer(0), m_playerPool(nullptr), 
m_waterPool(nullptr), m_tileSize(48.0f), m_levelNumber(1), m_layerNumber(0), m_hudParser(0), m_tileParser(0), m_weaponPool(nullptr), m_bulletPool(nullptr) {}

Level::~Level()
{
	delete m_playerPool;
	delete m_waterPool;
	delete m_hudParser;
	delete m_bulletPool;
	delete m_weaponPool;

	SoundSystem::getInstance().close();
}

bool Level::Initialise(Renderer& renderer)
{
	levelType = "summer";

	m_cooldownTime = 0.2f;
	m_currentCooldown = 0.0f;

	m_hudParser = new HUDParser();
	m_tileParser = new TileParser(levelType, 1);

	m_playerPool = new GameObjectPool(Player(), 4);
	m_weaponPool = new GameObjectPool(Weapon(), 6);
	m_bulletPool = new GameObjectPool(Bullet(), 20);

	if (!WeaponsInitialised(renderer)) {
		LogManager::GetInstance().Log("Unable to Initialise Weapons!");
		return false;
	}

	if (!BulletsInitialised(renderer)) {
		LogManager::GetInstance().Log("Unable to Initialise Bullets!");
		return false;
	}
	
	if (!PlayerInitialised(renderer)) {
		LogManager::GetInstance().Log("Unable to Initialise Player!");
		return false;
	}

	m_tileParser->Initialise(renderer);
	m_hudParser->Initialise(renderer);

	m_playerPosition = m_tileParser->GetPlayerStartPosition();
	m_waterPool = m_tileParser->GetWaterPool();
	
	LogManager::GetInstance().Log("Initialised all Sprites!");

	m_collisionTree = make_unique<QuadTree>(Box(0.0f, 0.0f, (float)renderer.GetWidth(), (float)renderer.GetHeight()));

	return true;

}

void Level::Process(float deltaTime, InputSystem& inputSystem)
{
	m_collisionTree->clear();

	if (m_currentCooldown > 0.0f) {
		m_currentCooldown -= deltaTime;
	}

	m_tileParser->Process(deltaTime, inputSystem);

	AddWaterCollision();

	PlayerMovement(inputSystem, m_currentPlayer, deltaTime);

	if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
		if (obj && dynamic_cast<Player*>(obj)) {
			Player* player = static_cast<Player*>(obj);
			player->Process(deltaTime);

			if (!player->IsRunning()) {
				m_currentPlayer = 0;
			}
		}
	}

	if (GameObject* obj = m_weaponPool->getObjectAtIndex(m_currentWeapon)) {
		if (obj && dynamic_cast<Weapon*>(obj)) {
			Weapon* weapon = static_cast<Weapon*>(obj);
			weapon->Process(deltaTime);

			weapon->Position() = m_playerPosition;
			weapon->Position().y += weapon->GetSpriteHeight() * 1.5f;
			weapon->Position().x += weapon->GetSpriteWidth() * 0.2f;
		}
	}

	for (size_t i = 0; i < m_bulletPool->totalCount(); i++) {
		if (GameObject* obj = m_bulletPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<Bullet*>(obj)) {
				Bullet* bullet = static_cast<Bullet*>(obj);
				bullet->Process(deltaTime);

				if (!bullet->isActive()) {
					m_bulletPool->release(bullet);
				}
			}
		}
	}

	m_hudParser->Process(deltaTime, inputSystem);
}

void Level::Draw(Renderer& renderer)
{
	m_tileParser->Draw(renderer);

	if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
		if (obj && obj->isActive()) {
			Player* player = static_cast<Player*>(obj);
			player->Draw(renderer);
		}
	}

	if (GameObject* obj = m_weaponPool->getObjectAtIndex(m_currentWeapon)) {
		if (obj && obj->isActive()) {
			Weapon* weapon = static_cast<Weapon*>(obj);
			weapon->Draw(renderer);
		}
	}

	for (size_t i = 0; i < m_bulletPool->totalCount(); i++) {
		if (GameObject* obj = m_bulletPool->getObjectAtIndex(i)) {
			if (obj && obj->isActive()) {
				Bullet* bullet = static_cast<Bullet*>(obj);
				bullet->Draw(renderer);
			}
		}
	}

	m_hudParser->Draw(renderer);
}

void Level::DebugDraw()
{
	if (ImGui::Begin("Weapon Debug")) {
		if (ImGui::Button("Basic Knife")) SetWeapon(0);
		if (ImGui::Button("Upgraded Knife")) SetWeapon(1);
		if (ImGui::Button("Basic Gun")) SetWeapon(2);
		if (ImGui::Button("Upgraded Gun")) SetWeapon(3);

		// Display current weapon stats
	}
	ImGui::End();
}

bool Level::PlayerInitialised(Renderer& renderer)
{
	for (size_t i = 0; i < m_playerPool->totalCount(); i++) {
		if (GameObject* obj = m_playerPool->getObjectAtIndex(i)) {
			Player* player = static_cast<Player*>(obj);

			switch (i) {
			case 0: // idle
				if (!player->Initialise(renderer, "..\\assets\\idle.png")) {
					return false;
				}

				player->SetActive(true);
				player->Position().x = m_playerPosition.x;
				player->Position().y = m_playerPosition.y;
				player->SetLoop(true);
				player->SetFrameTime(0.3f);

				break;
			case 1: // run
				if (!player->Initialise(renderer, "..\\assets\\run_right.png")) {
					return false;
				}

				player->SetActive(false);
				player->Position().x = m_playerPosition.x;
				player->Position().y = m_playerPosition.y;
				player->SetLoop(true);
				player->SetFrameTime(0.05f);
				break;

			case 2:
				if (!player->Initialise(renderer, "..\\assets\\run_left.png")) {
					return false;
				}

				player->SetActive(false);
				player->Position().x = m_playerPosition.x;
				player->Position().y = m_playerPosition.y;
				player->SetLoop(true);
				player->SetFrameTime(0.05f);
				break;

			case 3: // use
				if (!player->Initialise(renderer, "..\\assets\\use.png")) {
					return false;
				}

				player->SetActive(false);
				player->Position().x = m_playerPosition.x;
				player->Position().y = m_playerPosition.y;
				player->SetLoop(false);
				player->SetFrameTime(0.3f);
				break;
			default:
				return false;
			}
		}
	}

	return true;
}

void Level::PlayerMovement(InputSystem& inputSystem, int& m_currentPlayer, float deltaTime)
{
	m_playerPrevPosition = m_playerPosition;

	Vector2 updatedPos = m_playerPosition;

	if (inputSystem.GetKeyState(SDL_SCANCODE_RIGHT) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_RIGHT) == BS_PRESSED) {
		m_currentPlayer = 1;
		m_currentDirection = 'R';
		SwitchDirection(m_currentDirection);
		updatedPos.x += 80.0f * deltaTime;
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_LEFT) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_LEFT) == BS_PRESSED) {
		m_currentPlayer = 2;
		m_currentDirection = 'L';
		SwitchDirection(m_currentDirection);
		updatedPos.x -= 80.0f * deltaTime;
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_UP) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_UP) == BS_PRESSED) {

		if (m_currentPlayer != 2) {
			m_currentPlayer = 1;
		}

		updatedPos.y -= 80.0f * deltaTime;
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_DOWN) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_DOWN) == BS_PRESSED) {

		if (m_currentPlayer != 1) {
			m_currentPlayer = 2;
		}

		updatedPos.y += 80.0f * deltaTime;
	}

	//detect any collision from the potential movement:

	bool collision = false;

	Box updatedBox(updatedPos.x, updatedPos.y, m_playerSize, m_playerSize);

	auto potentialCollisions = m_collisionTree->queryRange(updatedBox);

	for (auto* obj : potentialCollisions) {
		Water* water = dynamic_cast<Water*>(obj);
		if (water && IsColliding(updatedBox, water)) {
			collision = true;
			break;
		}
	}

	if (!collision) {
		m_playerPosition = updatedPos;
	}
	else { // sliding so the player doesnt stop abruptly
	}

	if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
		Player* player = static_cast<Player*>(obj);
		player->Position() = m_playerPosition;
		player->SetActive(true);
	}

	if (inputSystem.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED) { // weapon swing
		m_currentPlayer = 3;
		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = static_cast<Player*>(obj);
			player->SetRunning();
			player->Position() = m_playerPosition;
			player->SetActive(true);
		}

		char weaponType;

		if (GameObject* obj = m_weaponPool->getObjectAtIndex(m_currentWeapon)) {
			Weapon* weapon = static_cast<Weapon*>(obj);
			weaponType = weapon->GetWeaponType();

			if (weaponType == 'M') {
				weapon->Swing();
			}
			else if (weaponType == 'G') {
				if (m_bulletPool->hasAvailableObjects()) {
					if (GameObject* obj = m_bulletPool->getObject()) {
						Bullet* bullet = static_cast<Bullet*>(obj);
						if (bullet && m_currentCooldown <= 0.0f) {
							bullet->Fire(weapon->Position(), inputSystem.GetMousePosition());
							m_currentCooldown = m_cooldownTime;
						}
					}
				}
			}
		}		
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_RIGHT) == BS_RELEASED || inputSystem.GetKeyState(SDL_SCANCODE_LEFT) == BS_RELEASED ||
		inputSystem.GetKeyState(SDL_SCANCODE_UP) == BS_RELEASED || inputSystem.GetKeyState(SDL_SCANCODE_DOWN) == BS_RELEASED) {
		m_currentPlayer = 3;
		m_currentDirection = 'R';
		SwitchDirection(m_currentDirection);

		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = static_cast<Player*>(obj);
			player->SetRunning();
		}

		m_currentPlayer = 0;

		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = static_cast<Player*>(obj);
			player->Position() = m_playerPosition;
			player->SetActive(true);
		}
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_E) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_E) == BS_PRESSED) {
		m_currentPlayer = 3;

		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = static_cast<Player*>(obj);
			player->SetRunning();
			player->Position() = m_playerPosition;
			player->SetActive(true);
		}
	}
}

bool Level::IsColliding(const Box& playerBox, Water* water)
{
	if (!water) return false;

	float wX = water->Position().x;
	float wY = water->Position().y;
	float wW = (float)water->GetSpriteWidth();

	return playerBox.x < wX + wW &&
		playerBox.x + playerBox.width > wX &&
		playerBox.y < wY + wW &&
		playerBox.y + playerBox.height > wY;
}

bool Level::WeaponsInitialised(Renderer& renderer)
{
	for (size_t i = 0; i < m_weaponPool->totalCount(); i++) {
		if (GameObject* obj = m_weaponPool->getObjectAtIndex(i)) {
			Weapon* weapon = static_cast<Weapon*>(obj);

			string filepath = "..\\assets\\";

			switch (i) {
			case 0:
				filepath += "basic_knife.png";
				break;
			case 1:
				filepath += "upgraded_knife.png";
				break;
			case 2:
				filepath += "basic_gun.png";
				break;
			case 3:
				filepath += "upgraded_gun.png";
				break;
			case 4:
				filepath += "basic_gun_left.png";
				break;
			case 5:
				filepath += "upgraded_gun_left.png";
				break;

			default:
				filepath += "basic_knife.png";
				break;
			}

			weapon->initialise(renderer, filepath.c_str());
			weapon->SetWeapon(true);

			switch (i) {
			case 0:
				weapon->SetWeaponType('M');
				break;
			case 1:
				weapon->SetWeaponType('M');
				break;
			case 2:
				weapon->SetWeaponType('G');
				break;
			case 3:
				weapon->SetWeaponType('G');
				break;
			case 4:
				weapon->SetWeaponType('G');
				break;
			case 5:
				weapon->SetWeaponType('G');
				break;

			default:
				weapon->SetWeaponType('M');
				break;
			}
		}
	}

	m_currentWeapon = 0;

	return true;
}

bool Level::BulletsInitialised(Renderer& renderer)
{
	for (size_t i = 0; i < m_bulletPool->totalCount(); i++) {
		if (GameObject* obj = m_bulletPool->getObjectAtIndex(i)) {
			Bullet* bullet = static_cast<Bullet*>(obj);
			if (!bullet->initialise(renderer)) {
				return false;
			}
			bullet->SetColour(0.2f, 0.5f, 0.5f);
		}
	}
	return true;
}

void Level::AddWaterCollision()
{
	for (size_t i = 0; i < m_waterPool->totalCount(); i++) {
		if (GameObject* obj = m_waterPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<Water*>(obj)) {
				Water* water = static_cast<Water*>(obj);

				if (water && water->isActive()) {
					float waterSize = (float)water->GetSpriteWidth();
					Box waterRange(
						water->Position().x,
						water->Position().y,
						waterSize,
						waterSize
					);

					m_collisionTree->insert(water, waterRange);
				}
			}
		}
	}
}

void Level::SetWeapon(int num)
{
	m_currentWeapon = num;
	m_hudParser->SetWeaponHUD(num);
}

void Level::SwitchDirection(char direction)
{
	if (direction == 'L') {
		switch (m_currentWeapon) {
		case 2:
			m_currentWeapon = 4;
			break;

		case 3:
			m_currentWeapon = 5;
			break;

		default:
			break;
		}
	}
	else if (direction == 'R') {
		switch (m_currentWeapon) {
		case 4: //basic left
			m_currentWeapon = 2;
			break;

		case 5: //upgraded left
			m_currentWeapon = 3;
			break;

		default:
			break;
		}
	}
}
