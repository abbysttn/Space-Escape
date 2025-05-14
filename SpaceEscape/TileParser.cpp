#include "TileParser.h"

#include "Renderer.h"
#include "DDLevelLoad.h"
#include "gameobjectpool.h"
#include "logmanager.h"
#include "quadtree.h"

#include "corner.h"
#include "edge.h"
#include "center.h"
#include "edgeCorner.h"
#include "water.h"

#include "prop.h"
#include "enemyspawner.h"

#include <cassert>

TileParser::TileParser(std::string lType, int lNum) : playerPositioned(false), m_playerStartPos(0,0), m_tileSize(48.0f), m_levelParser(0), m_centerPool(nullptr), m_cornerPool(nullptr), m_edgePool(nullptr), m_edgeCornerPool(nullptr),
m_waterPool(nullptr), m_bridgePool(nullptr), m_propPool(nullptr), m_spawnerPool(nullptr) {
	levelType = lType;
	levelNum = lNum;
}

TileParser::~TileParser()
{
	delete m_centerPool;
	delete m_cornerPool;
	delete m_edgePool;
	delete m_edgeCornerPool;
	delete m_waterPool;
	delete m_levelParser;
	delete m_bridgePool;
	delete m_propPool;
	delete m_spawnerPool;
}

bool TileParser::Initialise(Renderer& renderer)
{
	m_levelParser = new DDLevelLoad();

	m_cornerPool = new GameObjectPool(Corner(), 100);
	m_edgePool = new GameObjectPool(Edge(), 100);
	m_centerPool = new GameObjectPool(Center(), 100);
	m_edgeCornerPool = new GameObjectPool(EdgeCorner(), 100);
	m_waterPool = new GameObjectPool(Water(), 100);
	m_propPool = new GameObjectPool(Prop(), 100);
	m_spawnerPool = new GameObjectPool(EnemySpawner(), 3);

	if (!m_cornerPool || !m_edgePool || !m_centerPool || !m_edgeCornerPool || !m_waterPool || !m_propPool || !m_spawnerPool) {
		return false;
	}

	if (!FileParsed(renderer)) {
		LogManager::GetInstance().Log("File Failed to Parse!");
		return false;
	}

	return true;
}

void TileParser::Process(float deltaTime, InputSystem& inputSystem)
{
	if (!m_cornerPool || !m_edgePool || !m_centerPool || !m_edgeCornerPool || !m_waterPool || !m_propPool || !m_spawnerPool) {
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
			}
		}
	}

	for (size_t i = 0; i < m_waterPool->totalCount(); i++) {
		if (GameObject* obj = m_waterPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<Water*>(obj)) {
				Water* water = static_cast<Water*>(obj);
				water->Process(deltaTime);
			}
		}
	}

	for (size_t i = 0; i < m_propPool->totalCount(); i++) {
		if (GameObject* obj = m_propPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<Prop*>(obj)) {
				Prop* prop = static_cast<Prop*>(obj);
				prop->Process(deltaTime);
			}
		}
	}

	for (size_t i = 0; i < m_spawnerPool->totalCount(); i++) {
		if (GameObject* obj = m_spawnerPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<EnemySpawner*>(obj)) {
				EnemySpawner* spawner = static_cast<EnemySpawner*>(obj);
				spawner->Process(deltaTime);
			}
		}
	}
}

void TileParser::Draw(Renderer& renderer)
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

	for (size_t i = 0; i < m_propPool->totalCount(); i++) {
		if (GameObject* obj = m_propPool->getObjectAtIndex(i)) {
			if (obj && obj->isActive()) {
				Prop* prop = static_cast<Prop*>(obj);
				prop->Draw(renderer);
			}
		}
	}

	for (size_t i = 0; i < m_spawnerPool->totalCount(); i++) {
		if (GameObject* obj = m_spawnerPool->getObjectAtIndex(i)) {
			if (obj && obj->isActive()) {
				EnemySpawner* spawner = static_cast<EnemySpawner*>(obj);
				spawner->Draw(renderer);
			}
		}
	}
}

void TileParser::DebugDraw()
{
}

Vector2 TileParser::GetPlayerStartPosition()
{
	return m_playerStartPos;
}

GameObjectPool* TileParser::GetWaterPool()
{
	return m_waterPool;
}

GameObjectPool* TileParser::GetSpawnerPool()
{
	return m_spawnerPool;
}

bool TileParser::FileParsed(Renderer& renderer)
{
	for (int i = 0; i < 2; i++) {
		string levelPath = "..\\assets\\level" + to_string(levelNum);

		string levelPathLayer = levelPath + "_" + to_string(i) + ".txt";

		if (!m_levelParser->LoadLevelFile(levelPathLayer)) {
			LogManager::GetInstance().Log("Level Failed To Load!");
			return false;
		}

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

				if (!InitObjects(renderer, tileType, x, y, i)) {
					LogManager::GetInstance().Log("Unable to Initialise all Sprites!");
					return false;
				}
			}
		}
	}

	return true;
}

bool TileParser::InitObjects(Renderer& renderer, char tileType, size_t x, size_t y, int layer)
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

				//set player on a center tile
				if (!playerPositioned) {
					m_playerStartPos = center->Position();
					playerPositioned = true;
				}
			}
		}
		return true;

	case ' ':
		if (layer == 0) {
			if (GameObject* obj = m_waterPool->getObject()) {
				Water* water = dynamic_cast<Water*>(obj);

				if (water) {
					string filepath = "..\\assets\\water_" + levelType + ".png";

					if (!water->initialise(renderer, filepath.c_str())) {
						return false;
					}

					water->Position().x = (x * m_tileSize) + screenOffsetX;
					water->Position().y = (y * m_tileSize) + screenOffsetY;
					water->SetActive(true);
				}
			}
		}

		return true;

	case '1':
		if (GameObject* obj = m_propPool->getObject()) {
			Prop* prop = dynamic_cast<Prop*>(obj);

			if (prop) {

				if (!prop->initialise(renderer, levelType)) {
					return false;
				}

				prop->Position().x = (x * m_tileSize) + screenOffsetX;
				prop->Position().y = (y * m_tileSize) + screenOffsetY;
				prop->SetActive(true);
			}
		}
		return true;

	case '+':
		if (GameObject* obj = m_spawnerPool->getObject()) {
			EnemySpawner* spawner = dynamic_cast<EnemySpawner*>(obj);

			if (spawner) {

				if (!spawner->Initialise(renderer)) {
					return false;
				}

				spawner->Position().x = (x * m_tileSize) + screenOffsetX;
				spawner->Position().y = (y * m_tileSize) + screenOffsetY;
				spawner->SetActive(true);
			}
		}
		return true;

	default:
		return false;
	}
}
