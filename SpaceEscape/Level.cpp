#include "Level.h"

#include "Renderer.h"
#include "DDLevelLoad.h"
#include "gameobjectpool.h"
#include "logmanager.h"

#include "corner.h"

Level::Level() : m_centerPool(nullptr), m_cornerPool(nullptr), m_edgePool(nullptr), m_levelParser(0), m_tileSize(16.0f) {}

Level::~Level()
{
	delete m_centerPool;
	delete m_cornerPool;
	delete m_edgePool;
	delete m_levelParser;
}

bool Level::Initialise(Renderer& renderer)
{
	m_levelParser = new DDLevelLoad();

	m_cornerPool = new GameObjectPool(Corner(), 100);

	if (!m_cornerPool) {
		return false;
	}

	if (!m_levelParser->LoadLevelFile("..\\assets\\level.txt")) {
		LogManager::GetInstance().Log("Level Failed To Load!");
		return false;
	}

	size_t tileCount = m_levelParser->GetWidth() * m_levelParser->GetHeight();

	float screenWidth = renderer.GetWidth();
	float screenHeight = renderer.GetHeight();

	size_t levelWidth = m_levelParser->GetWidth();
	size_t levelHeight = m_levelParser->GetHeight();

	float levelPixelWidth = levelWidth * m_tileSize;
	float levelPixelHeight = levelHeight * m_tileSize;

	float screenOffsetX = ((screenWidth - levelPixelWidth) / 2.0f) + (m_tileSize / 2.0f);
	float screenOffsetY = ((screenHeight - levelPixelHeight) / 2.0f) + (m_tileSize / 2.0f);

	for (size_t y = 0; y < m_levelParser->GetHeight(); y++) {
		for (size_t x = 0; x < m_levelParser->GetWidth(); x++) {
			char tileType = m_levelParser->GetTileAt(x, y);
			float posX = x * m_tileSize;
			float posY = y * m_tileSize;

			if (tileType == 'W') {

				if (tileType == 'W') {
					if (GameObject* obj = m_cornerPool->getObject()) {
						Corner* corner = dynamic_cast<Corner*>(obj);

						if (corner) {
							if (!corner->initialise(renderer)) {
								return false;
							}

							corner->Position().x = (x * m_tileSize) + screenOffsetX;
							corner->Position().y = (y * m_tileSize) + screenOffsetY;
							corner->SetActive(true);

							LogManager::GetInstance().Log("Wall Added!");
						}
					}
				}
			}
		}
	}


	return true;

}

void Level::Process(float deltaTime, InputSystem& inputSystem)
{
	if (!m_cornerPool) {
		return;
	}

	for (size_t i = 0; i < m_cornerPool->totalCount(); i++) {
		if (GameObject* obj = m_cornerPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<Corner*>(obj)) {
				Corner* corner = static_cast<Corner*>(obj);
				corner->Process(deltaTime);
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
}

void Level::DebugDraw()
{
}
