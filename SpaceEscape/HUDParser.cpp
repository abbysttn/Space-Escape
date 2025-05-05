#include "HUDParser.h"

#include "Renderer.h"
#include "DDLevelLoad.h"
#include "gameobjectpool.h"
#include "logmanager.h"

#include "underlayTiles.h"

#include <cassert>

HUDParser::HUDParser() : m_HUDUnderlayTiles(nullptr), m_levelParser(0) {}

HUDParser::~HUDParser()
{
	delete m_HUDUnderlayTiles;
	delete m_levelParser;
}

bool HUDParser::Initialise(Renderer& renderer)
{
	m_levelParser = new DDLevelLoad();

	m_HUDUnderlayTiles = new GameObjectPool(UnderlayTiles(), 100);

	if (!FileParsed(renderer, "..\\assets\\hud_underlay.txt")) {
		LogManager::GetInstance().Log("File Failed to Parse!");
		return false;
	}

	if (!FileParsed(renderer, "..\\assets\\hud_overlay.txt")) {
		LogManager::GetInstance().Log("File Failed to Parse!");
		return false;
	}
	

	return false;
}

void HUDParser::Process(float deltaTime, InputSystem& inputSystem)
{
	for (size_t i = 0; i < m_HUDUnderlayTiles->totalCount(); i++) {
		if (GameObject* obj = m_HUDUnderlayTiles->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<UnderlayTiles*>(obj)) {
				UnderlayTiles* tile = static_cast<UnderlayTiles*>(obj);
				tile->Process(deltaTime);
			}
		}
	}
}

void HUDParser::Draw(Renderer& renderer)
{
	for (size_t i = 0; i < m_HUDUnderlayTiles->totalCount(); i++) {
		if (GameObject* obj = m_HUDUnderlayTiles->getObjectAtIndex(i)) {
			if (obj && obj->isActive()) {
				UnderlayTiles* tile = static_cast<UnderlayTiles*>(obj);
				tile->Draw(renderer);
			}
		}
	}
}

void HUDParser::DebugDraw()
{
}

bool HUDParser::FileParsed(Renderer& renderer, const char* filepath)
{
	if (!m_levelParser->LoadLevelFile(filepath)) {
		LogManager::GetInstance().Log("Level Failed To Load!");
		return false;
	}

	float screenWidth = (float)renderer.GetWidth();
	float screenHeight = (float)renderer.GetHeight();

	size_t levelWidth = m_levelParser->GetWidth();
	size_t levelHeight = m_levelParser->GetHeight();

	float levelPixelWidth = (levelWidth * 48.0f);
	float levelPixelHeight = (levelHeight * 48.0f);

	screenOffsetX = (48.0f / 2.0f) + 24.0f; //center tile + offset
	screenOffsetXR = screenWidth - levelPixelWidth + 48.0f; // right aligned
	screenOffsetY = (48.0f / 2.0f) + 24.0f;
	screenOffsetXR = screenHeight - levelPixelHeight; // bottom aligned

	for (size_t y = 0; y < m_levelParser->GetHeight(); y++) {
		for (size_t x = 0; x < m_levelParser->GetWidth(); x++) {
			char tileType = m_levelParser->GetTileAt(x, y);

			if (!InitObjects(renderer, tileType, x, y)) {
				LogManager::GetInstance().Log("Unable to Initialise all Sprites!");
				return false;
			}
		}
	}

	return true;
}

bool HUDParser::InitObjects(Renderer& renderer, char tileType, size_t x, size_t y)
{
	switch (tileType) {
	case 'A':
	case 'B':
	case 'C':
	case 'D':
		if (GameObject* obj = m_HUDUnderlayTiles->getObject()) {
			UnderlayTiles* tile = dynamic_cast<UnderlayTiles*>(obj);

			if (tile) {
				string filepath = "..\\assets\\hud_corner.png";

				if (!tile->initialise(renderer, filepath.c_str())) {
					return false;
				}

				tile->Position().x = (x * 48.0f) + screenOffsetXR;
				tile->Position().y = (y * 48.0f) + screenOffsetY;

				switch (tileType) {
				case 'B':
					tile->SetRotation(90.0f);
					break;
				case 'C':
					tile->SetRotation(180.0f);
					break;
				case 'D':
					tile->SetRotation(-90.0f);
					break;
				}

				tile->SetActive(true);
			}
		}
		return true;

	case 'E':
	case 'F':
	case 'G':
	case 'H':
		if (GameObject* obj = m_HUDUnderlayTiles->getObject()) {
			UnderlayTiles* tile = dynamic_cast<UnderlayTiles*>(obj);

			if (tile) {
				string filepath = "..\\assets\\hud_middle.png";

				if (!tile->initialise(renderer, filepath.c_str())) {
					return false;
				}

				tile->Position().x = (x * 48.0f) + screenOffsetXR;
				tile->Position().y = (y * 48.0f) + screenOffsetY;

				switch (tileType) {
				case 'F':
					tile->SetRotation(90.0f);
					break;
				case 'G':
					tile->SetRotation(180.0f);
					break;
				case 'H':
					tile->SetRotation(-90.0f);
					break;
				}

				tile->SetActive(true);
			}
		}
		return true;

	case 'Z':
	case 'Y':
		if (GameObject* obj = m_HUDUnderlayTiles->getObject()) {
			UnderlayTiles* tile = dynamic_cast<UnderlayTiles*>(obj);

			if (tile) {
				string filepath = "..\\assets\\hud_center.png";

				if (!tile->initialise(renderer, filepath.c_str())) {
					return false;
				}

				if (tileType == 'Y') {
					tile->Position().x = (x * 48.0f) + screenOffsetXR;
				}
				else {
					tile->Position().x = (x * 48.0f) + screenOffsetX;
				}

				tile->Position().y = (y * 48.0f) + screenOffsetY;

				tile->SetActive(true);
			}
		}
		return true;

	case 'J':
	case 'K':
		if (GameObject* obj = m_HUDUnderlayTiles->getObject()) {
			UnderlayTiles* tile = dynamic_cast<UnderlayTiles*>(obj);

			if (tile) {
				string filepath = "..\\assets\\hud_player_corner_left.png";

				if (!tile->initialise(renderer, filepath.c_str())) {
					return false;
				}

				tile->Position().x = (x * 48.0f) + screenOffsetX;
				tile->Position().y = (y * 48.0f) + screenOffsetY;

				if (tileType == 'K') {
					tile->SetRotation(180.0f);
				}

				tile->SetActive(true);
			}
		}
		return true;

	case 'L':
	case 'M':
		if (GameObject* obj = m_HUDUnderlayTiles->getObject()) {
			UnderlayTiles* tile = dynamic_cast<UnderlayTiles*>(obj);

			if (tile) {
				string filepath = "..\\assets\\hud_player_corner_right.png";

				if (!tile->initialise(renderer, filepath.c_str())) {
					return false;
				}

				tile->Position().x = (x * 48.0f) + screenOffsetX;
				tile->Position().y = (y * 48.0f) + screenOffsetY;

				if (tileType == 'M') {
					tile->SetRotation(180.0f);
				}

				tile->SetActive(true);
			}
		}
		return true;

	case 'N':
	case 'O':
		if (GameObject* obj = m_HUDUnderlayTiles->getObject()) {
			UnderlayTiles* tile = dynamic_cast<UnderlayTiles*>(obj);

			if (tile) {
				string filepath = "..\\assets\\hud_player_side.png";

				if (!tile->initialise(renderer, filepath.c_str())) {
					return false;
				}

				tile->Position().x = (x * 48.0f) + screenOffsetX;
				tile->Position().y = (y * 48.0f) + screenOffsetY;

				if (tileType == 'O') {
					tile->SetRotation(180.0f);
				}

				tile->SetActive(true);
			}
		}
		return true;

	case 'P':
	case 'Q':
		if (GameObject* obj = m_HUDUnderlayTiles->getObject()) {
			UnderlayTiles* tile = dynamic_cast<UnderlayTiles*>(obj);

			if (tile) {
				string filepath = "..\\assets\\hud_player_middle.png";

				if (!tile->initialise(renderer, filepath.c_str())) {
					return false;
				}

				tile->Position().x = (x * 48.0f) + screenOffsetX;
				tile->Position().y = (y * 48.0f) + screenOffsetY;

				if (tileType == 'Q') {
					tile->SetRotation(180.0f);
				}

				tile->SetActive(true);
			}
		}
		return true;

	case '1':
	case '2':
	case '3':
	case '4':
		if (GameObject* obj = m_HUDUnderlayTiles->getObject()) {
			UnderlayTiles* tile = dynamic_cast<UnderlayTiles*>(obj);

			if (tile) {
				string filepath;

				if (tileType == '1') {
					filepath = "..\\assets\\player_top_left.png";
				}
				else if (tileType == '2') {
					filepath = "..\\assets\\player_top_right.png";
				}
				else if (tileType == '3') {
					filepath = "..\\assets\\player_bottom_left.png";
				}
				else {
					filepath = "..\\assets\\player_bottom_right.png";
				}
				
				if (!tile->initialise(renderer, filepath.c_str())) {
					return false;
				}

				tile->Position().x = (x * 48.0f) + screenOffsetX + 25.0f;
				tile->Position().y = (y * 48.0f) + screenOffsetY + 25.0f;

				tile->SetActive(true);
			}
		}
		return true;

	case ' ':
		return true;


	default:
		return false;
	}
}
