#include "Level.h"

#include "Renderer.h"
#include "DDLevelLoad.h"
#include "gameobjectpool.h"
#include "logmanager.h"
#include "inputsystem.h"
#include "quadtree.h"
#include "soundsystem.h"

#include "corner.h"
#include "edge.h"
#include "center.h"
#include "edgeCorner.h"
#include "water.h"
#include "player.h"

Level::Level() : m_soundSystem(0), m_playerPrevPosition(800, 500), m_playerPosition(800, 500), m_currentPlayer(0), m_playerPool(nullptr), 
m_centerPool(nullptr), m_cornerPool(nullptr), m_edgePool(nullptr), m_edgeCornerPool(nullptr), m_waterPool(nullptr), m_levelParser(0), m_tileSize(48.0f) {
	SoundSystem::getInstance().initialise();
}

Level::~Level()
{
	delete m_centerPool;
	delete m_cornerPool;
	delete m_edgePool;
	delete m_edgeCornerPool;
	delete m_waterPool;
	delete m_levelParser;
	delete m_playerPool;

	SoundSystem::getInstance().close();
}

bool Level::Initialise(Renderer& renderer)
{
	levelType = "summer";

	m_soundSystem = new SoundSystem();
	m_soundSystem->getInstance().loadSound("run", "..\\assets\\run.mp3", false);

	m_levelParser = new DDLevelLoad();

	m_cornerPool = new GameObjectPool(Corner(), 100);
	m_edgePool = new GameObjectPool(Edge(), 100);
	m_centerPool = new GameObjectPool(Center(), 100);
	m_edgeCornerPool = new GameObjectPool(EdgeCorner(), 100);
	m_waterPool = new GameObjectPool(Water(), 100);
	m_playerPool = new GameObjectPool(Player(), 4);

	if (!m_cornerPool || !m_edgePool || !m_centerPool || !m_edgeCornerPool || !m_waterPool) {
		return false;
	}

	if (!m_levelParser->LoadLevelFile("..\\assets\\level.txt")) {
		LogManager::GetInstance().Log("Level Failed To Load!");
		return false;
	}

	size_t tileCount = m_levelParser->GetWidth() * m_levelParser->GetHeight();

	float screenWidth = (float)renderer.GetWidth();
	float screenHeight = (float)renderer.GetHeight();

	size_t levelWidth = m_levelParser->GetWidth();
	size_t levelHeight = m_levelParser->GetHeight();

	float levelPixelWidth = levelWidth * m_tileSize;
	float levelPixelHeight = levelHeight * m_tileSize;

	screenOffsetX = ((screenWidth - levelPixelWidth) / 2.0f) + (m_tileSize / 2.0f);
	screenOffsetY = ((screenHeight - levelPixelHeight) / 2.0f) + (m_tileSize / 2.0f);

	for (size_t y = 0; y < m_levelParser->GetHeight(); y++) {
		for (size_t x = 0; x < m_levelParser->GetWidth(); x++) {
			char tileType = m_levelParser->GetTileAt(x, y);

			if (!InitObjects(renderer, tileType, x, y)) {
				LogManager::GetInstance().Log("Unable to Initialise all Sprites!");
				return false;
			}
		}
	}
	
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

	LogManager::GetInstance().Log("Initialised all Sprites!");

	m_collisionTree = make_unique<QuadTree>(Box(0, 0, screenWidth, renderer.GetHeight()));

	return true;

}

void Level::Process(float deltaTime, InputSystem& inputSystem)
{
	if (!m_cornerPool || !m_edgePool || !m_centerPool || !m_edgeCornerPool || !m_waterPool) {
		return;
	}

	m_collisionTree->clear();

	PlayerMovement(inputSystem, m_currentPlayer, deltaTime);

	for (size_t i = 0; i < m_cornerPool->totalCount(); i++) {
		if (GameObject* obj = m_cornerPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<Corner*>(obj)) {
				Corner* corner = static_cast<Corner*>(obj);
				corner->Process(deltaTime);
			}
		}
	}

	for (size_t i = 0; i < m_edgePool->totalCount(); i++) {
		if (GameObject* obj = m_edgePool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<Edge*>(obj)) {
				Edge* edge = static_cast<Edge*>(obj);
				edge->Process(deltaTime);
			}
		}
	}

	for (size_t i = 0; i < m_centerPool->totalCount(); i++) {
		if (GameObject* obj = m_centerPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<Center*>(obj)) {
				Center* center = static_cast<Center*>(obj);
				center->Process(deltaTime);
			}
		}
	}

	for (size_t i = 0; i < m_edgeCornerPool->totalCount(); i++) {
		if (GameObject* obj = m_edgeCornerPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<EdgeCorner*>(obj)) {
				EdgeCorner* edgeCorner = static_cast<EdgeCorner*>(obj);
				edgeCorner->Process(deltaTime);
			}
		}
	}

	for (size_t i = 0; i < m_waterPool->totalCount(); i++) {
		if (GameObject* obj = m_waterPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<Water*>(obj)) {
				Water* water = static_cast<Water*>(obj);
				water->Process(deltaTime);

				if (water && water->isActive()) {
					float waterSize = (float)water->GetSpriteWidth();
					Box waterRange(
						water->Position().x - waterSize / 2,
						water->Position().y - waterSize / 2,
						waterSize,
						waterSize
					);

					m_collisionTree->insert(water, waterRange);
				}
			}
		}
	}

	if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
		if (obj && dynamic_cast<Player*>(obj)) {
			Player* player = static_cast<Player*>(obj);
			player->Process(deltaTime);

			if (!player->IsRunning()) {
				m_currentPlayer = 0;
			}

			float playerSize = player->GetSpriteWidth();
			Box playerBox(
				player->Position().x - playerSize / 2,
				player->Position().y - playerSize / 2,
				playerSize,
				playerSize
			);

			auto potentialCollisions = m_collisionTree->queryRange(playerBox);

			for (auto* obj : potentialCollisions) {
				Water* water = dynamic_cast<Water*>(obj);
				if (water) {
					if (IsColliding(player, water)) {

						if (!IsMovingAway(water)) {
							m_playerPosition = m_playerPrevPosition;
							player->Position() = m_playerPrevPosition;
						}

						break;
					}
				}
			}
		}
	}
}

void Level::Draw(Renderer& renderer)
{
	for (size_t i = 0; i < m_cornerPool->totalCount(); i++) {
		if (GameObject* obj = m_cornerPool->getObjectAtIndex(i)) {
			if (obj && obj->isActive()) {
				Corner* corner = static_cast<Corner*>(obj);
				corner->Draw(renderer);
			}
		}
	}

	for (size_t i = 0; i < m_edgePool->totalCount(); i++) {
		if (GameObject* obj = m_edgePool->getObjectAtIndex(i)) {
			if (obj && obj->isActive()) {
				Edge* edge = static_cast<Edge*>(obj);
				edge->Draw(renderer);
			}
		}
	}

	for (size_t i = 0; i < m_centerPool->totalCount(); i++) {
		if (GameObject* obj = m_centerPool->getObjectAtIndex(i)) {
			if (obj && obj->isActive()) {
				Center* center = static_cast<Center*>(obj);
				center->Draw(renderer);
			}
		}
	}

	for (size_t i = 0; i < m_edgeCornerPool->totalCount(); i++) {
		if (GameObject* obj = m_edgeCornerPool->getObjectAtIndex(i)) {
			if (obj && obj->isActive()) {
				EdgeCorner* edgeCorner = static_cast<EdgeCorner*>(obj);
				edgeCorner->Draw(renderer);
			}
		}
	}

	for (size_t i = 0; i < m_waterPool->totalCount(); i++) {
		if (GameObject* obj = m_waterPool->getObjectAtIndex(i)) {
			if (obj && obj->isActive()) {
				Water* water = static_cast<Water*>(obj);
				water->Draw(renderer);
			}
		}
	}

	if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
		if (obj && obj->isActive()) {
			Player* player = static_cast<Player*>(obj);
			player->Draw(renderer);
		}
	}
}

void Level::DebugDraw()
{
}

bool Level::InitObjects(Renderer& renderer, char tileType, size_t x, size_t y)
{
	switch (tileType) {
	case 'A':
	case 'B':
	case 'C':
	case 'D':
		if (GameObject* obj = m_cornerPool->getObject()) {
			Corner* corner = dynamic_cast<Corner*>(obj);

			if (corner) {
				string filepath = "..\\assets\\corner_" + levelType + ".png";

				if (!corner->initialise(renderer, filepath.c_str())) {
					return false;
				}

				corner->Position().x = (x * m_tileSize) + screenOffsetX;
				corner->Position().y = (y * m_tileSize) + screenOffsetY;

				switch (tileType) {
				case 'B':
					corner->SetRotation(90.0f);
					break;
				case 'C':
					corner->SetRotation(180.0f);
					break;
				case 'D':
					corner->SetRotation(-90.0f);
					break;
				}
	
				corner->SetActive(true);
			}
		}
		return true;

	case 'E':
	case 'F':
	case 'G':
	case 'H':
		if (GameObject* obj = m_edgePool->getObject()) {
			Edge* edge = dynamic_cast<Edge*>(obj);

			if (edge) {
				string filepath = "..\\assets\\edge_" + levelType + ".png";

				if (!edge->initialise(renderer, filepath.c_str())) {
					return false;
				}

				edge->Position().x = (x * m_tileSize) + screenOffsetX;
				edge->Position().y = (y * m_tileSize) + screenOffsetY;
				
				switch (tileType) {
				case 'F':
					edge->SetRotation(90.0f);
					break;
				case 'G':
					edge->SetRotation(180.0f);
					break;
				case 'H':
					edge->SetRotation(-90.0f);
					break;
				}

				edge->SetActive(true);
			}
		}
		return true;

	case 'I':
	case 'J':
	case 'K':
	case 'L':
		if (GameObject* obj = m_edgeCornerPool->getObject()) {
			EdgeCorner* edgeCorner = dynamic_cast<EdgeCorner*>(obj);

			if (edgeCorner) {
				string filepath = "..\\assets\\edge_corner_" + levelType + ".png";

				if (!edgeCorner->initialise(renderer, filepath.c_str())) {
					return false;
				}

				edgeCorner->Position().x = (x * m_tileSize) + screenOffsetX;
				edgeCorner->Position().y = (y * m_tileSize) + screenOffsetY;

				switch (tileType) {
				case 'J':
					edgeCorner->SetRotation(90.0f);
					break;
				case 'K':
					edgeCorner->SetRotation(180.0f);
					break;
				case 'L':
					edgeCorner->SetRotation(-90.0f);
					break;
				}

				edgeCorner->SetActive(true);
			}
		}
		return true;

	case 'O':
		if (GameObject* obj = m_centerPool->getObject()) {
			Center* center = dynamic_cast<Center*>(obj);

			if (center) {
				string filepath = "..\\assets\\center_" + levelType + ".png";

				if (!center->initialise(renderer, filepath.c_str())) {
					return false;
				}

				center->Position().x = (x * m_tileSize) + screenOffsetX;
				center->Position().y = (y * m_tileSize) + screenOffsetY;
				center->SetActive(true);
			}
		}
		return true;

	case ' ':
		if (GameObject* obj = m_waterPool->getObject()) {
			Water* water = dynamic_cast<Water*>(obj);

			if (water) {
				string filepath = "..\\assets\\center_" + levelType + ".png";

				if (!water->initialise(renderer, filepath.c_str())) {
					return false;
				}

				water->Position().x = (x * m_tileSize) + screenOffsetX;
				water->Position().y = (y * m_tileSize) + screenOffsetY;
				water->SetActive(true);
			}
		}
		return true;

	default:
		return false;
	}
}

void Level::PlayerMovement(InputSystem& inputSystem, int& m_currentPlayer, float deltaTime)
{
	m_playerPrevPosition = m_playerPosition;

	if (inputSystem.GetKeyState(SDL_SCANCODE_RIGHT) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_RIGHT) == BS_PRESSED) {
		m_currentPlayer = 1;

		

		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = static_cast<Player*>(obj);
			//m_soundSystem->getInstance().playSound("run", 1.0f);
			player->Position() = m_playerPosition;
			player->Position().x += 80.0f * deltaTime;
			m_playerPosition = player->Position();
			player->SetActive(true);
		}
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_LEFT) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_LEFT) == BS_PRESSED) {
		m_currentPlayer = 2;

		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = static_cast<Player*>(obj);
			player->Position() = m_playerPosition;
			player->Position().x -= 80.0f * deltaTime;
			m_playerPosition = player->Position();
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

	if (inputSystem.GetKeyState(SDL_SCANCODE_UP) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_UP) == BS_PRESSED) {

		if (m_currentPlayer != 2) {
			m_currentPlayer = 1;
		}

		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = static_cast<Player*>(obj);
			player->Position() = m_playerPosition;
			player->Position().y -= 80.0f * deltaTime;
			m_playerPosition = player->Position();
			player->SetActive(true);
		}
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_DOWN) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_DOWN) == BS_PRESSED) {

		if (m_currentPlayer != 1) {
			m_currentPlayer = 2;
		}

		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = static_cast<Player*>(obj);
			player->Position() = m_playerPosition;
			player->Position().y += 80.0f * deltaTime;
			m_playerPosition = player->Position();
			player->SetActive(true);
		}
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_RIGHT) == BS_RELEASED || inputSystem.GetKeyState(SDL_SCANCODE_LEFT) == BS_RELEASED ||
		inputSystem.GetKeyState(SDL_SCANCODE_UP) == BS_RELEASED || inputSystem.GetKeyState(SDL_SCANCODE_DOWN) == BS_RELEASED) {
		m_currentPlayer = 3;

		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = static_cast<Player*>(obj);
			player->SetRunning();
		}

		m_currentPlayer = 0;

		//m_soundSystem->stopSound("run");

		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = static_cast<Player*>(obj);
			player->Position() = m_playerPosition;
			player->SetActive(true);
		}
	}
}

bool Level::IsColliding(Player* player, Water* water)
{
	if (!player || !water) return false;

	float playerHalfWidth = player->GetSpriteWidth() / 2.0f;
	float tileHalfWidth = water->GetSpriteWidth() / 2.0f;

	float dx = fabs(player->Position().x - water->Position().x);
	float dy = fabs(player->Position().y - water->Position().y);

	return (dx < playerHalfWidth + tileHalfWidth) && (dy < playerHalfWidth + tileHalfWidth);
}

bool Level::IsMovingAway(Water* water)
{
	float dx = m_playerPosition.x - m_playerPrevPosition.x;
	float dy = m_playerPosition.y - m_playerPrevPosition.y;

	float waterDirX = m_playerPosition.x - water->Position().x;
	float waterDirY = m_playerPosition.y - water->Position().y;

	return (dx * waterDirX + dy * waterDirY) > 0;
}
