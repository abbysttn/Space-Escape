#include "Level.h"

#include "Renderer.h"
#include "DDLevelLoad.h"
#include "gameobjectpool.h"
#include "logmanager.h"
#include "inputsystem.h"
#include "quadtree.h"
#include "soundsystem.h"

#include "HUDParser.h"
#include "TileParser.h"

#include "water.h"
#include "player.h"
#include "weapon.h"

Level::Level() : m_playerSize(48.0f), m_soundSystem(0), m_playerPrevPosition(0, 0), m_playerPosition(0, 0), m_currentPlayer(0), m_playerPool(nullptr), 
m_waterPool(nullptr), m_tileSize(48.0f), m_levelNumber(1), m_layerNumber(0), m_hudParser(0), m_tileParser(0), m_weapon(0) {}

Level::~Level()
{
	delete m_playerPool;
	delete m_waterPool;
	delete m_hudParser;

	SoundSystem::getInstance().close();
}

bool Level::Initialise(Renderer& renderer)
{
	levelType = "summer";

	m_soundSystem = new SoundSystem();
	m_soundSystem->getInstance().initialise();
	m_soundSystem->getInstance().loadSound("run", "..\\assets\\run.mp3", false);

	m_hudParser = new HUDParser();
	m_tileParser = new TileParser(levelType, 1);

	m_playerPool = new GameObjectPool(Player(), 4);
	m_weapon = new Weapon();
	
	if (!PlayerInitialised(renderer)) {
		LogManager::GetInstance().Log("Unable to Initialise Player!");
		return false;
	}

	m_tileParser->Initialise(renderer);
	m_hudParser->Initialise(renderer);
	
	LogManager::GetInstance().Log("Initialised all Sprites!");

	m_playerPosition = m_tileParser->GetPlayerStartPosition();
	m_waterPool = m_tileParser->GetWaterPool();
	m_weapon->initialise(renderer, "..\\assets\\upgraded_knife.png");
	m_weapon->SetWeapon(true);

	m_collisionTree = make_unique<QuadTree>(Box(0.0f, 0.0f, (float)renderer.GetWidth(), (float)renderer.GetHeight()));

	return true;

}

void Level::Process(float deltaTime, InputSystem& inputSystem)
{
	m_collisionTree->clear();

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

	m_weapon->Position() = m_playerPosition;
	m_weapon->Position().y += m_weapon->GetSpriteHeight() * 1.5f;
	m_weapon->Position().x += m_weapon->GetSpriteWidth() * 0.2f;
	m_weapon->Process(deltaTime);

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

	m_weapon->Draw(renderer);

	m_hudParser->Draw(renderer);
}

void Level::DebugDraw()
{
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
		m_weapon->SetRotation(0.0f);
		updatedPos.x += 80.0f * deltaTime;
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_LEFT) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_LEFT) == BS_PRESSED) {
		m_currentPlayer = 2;
		m_weapon->SetRotation(180.0f);
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
		m_weapon->Swing();
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_RIGHT) == BS_RELEASED || inputSystem.GetKeyState(SDL_SCANCODE_LEFT) == BS_RELEASED ||
		inputSystem.GetKeyState(SDL_SCANCODE_UP) == BS_RELEASED || inputSystem.GetKeyState(SDL_SCANCODE_DOWN) == BS_RELEASED) {
		m_currentPlayer = 3;
		m_weapon->SetRotation(0.0f);//

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
