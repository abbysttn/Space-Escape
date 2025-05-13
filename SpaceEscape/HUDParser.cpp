#include "HUDParser.h"

#include "Renderer.h"
#include "DDLevelLoad.h"
#include "gameobjectpool.h"
#include "logmanager.h"
#include "weapon.h"
#include "life.h"

#include "playerstate.h"
#include "itemmanager.h"
#include "hudshippart.h"

#include "underlayTiles.h"

#include <cassert>

HUDParser::HUDParser() : m_HUDTiles(nullptr), m_levelParser(0), m_tileSize(48.0f), m_weaponPool(nullptr), m_playerLivesPool(nullptr), m_playerHealthPool(nullptr), m_itemDisplayPool(nullptr),
m_itemPool(nullptr) {}

HUDParser::~HUDParser()
{
	delete m_HUDTiles;
	delete m_levelParser;
	delete m_weaponPool;
	delete m_playerLivesPool;
	delete m_playerHealthPool;
	delete m_itemPool;
}

bool HUDParser::Initialise(Renderer& renderer)
{
	m_levelParser = new DDLevelLoad();

	m_itemDisplayPool = ItemManager::GetInstance().GetPool();
	m_itemPool = new GameObjectPool(HUDShipPart(), 7);

	m_HUDTiles = new GameObjectPool(UnderlayTiles(), 100);
	m_weaponPool = new GameObjectPool(Weapon(), 4);
	m_playerLivesPool = new GameObjectPool(Life(), 3);
	m_playerHealthPool = new GameObjectPool(UnderlayTiles(), 10);

	if (!FileParsed(renderer, "..\\assets\\hud_underlay.txt")) {
		LogManager::GetInstance().Log("File Failed to Parse!");
		return false;
	}

	if (!FileParsed(renderer, "..\\assets\\hud_overlay.txt")) {
		LogManager::GetInstance().Log("File Failed to Parse!");
		return false;
	}

	if (!WeaponsInitialised(renderer)) {
		LogManager::GetInstance().Log("Unable to Initialise Weapons!");
		return false;
	}

	if (!LivesInitialised(renderer)) {
		LogManager::GetInstance().Log("Unable to Initialise Lives!");
		return false;
	}

	PositionItems(renderer);

	return true;
}

void HUDParser::Process(float deltaTime, InputSystem& inputSystem)
{
	for (size_t i = 0; i < m_HUDTiles->totalCount(); i++) {
		if (GameObject* obj = m_HUDTiles->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<UnderlayTiles*>(obj)) {
				UnderlayTiles* tile = static_cast<UnderlayTiles*>(obj);
				tile->Process(deltaTime);
			}
		}
	}

	for (size_t i = 0; i < m_itemPool->totalCount(); i++) {
		if (GameObject* obj = m_itemPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<HUDShipPart*>(obj)) {
				HUDShipPart* HUDpart = static_cast<HUDShipPart*>(obj);
				HUDpart->Process(deltaTime);
				HUDpart->SetColour(0.0f, 0.0f, 0.0f);

				bool collected = false;

				if (GameObject* obj2 = m_itemDisplayPool->getObjectAtIndex(i)) {
					if (obj2 && dynamic_cast<ShipPart*>(obj2)) {
						ShipPart* part = static_cast<ShipPart*>(obj2);
						if (ItemManager::GetInstance().IsCollected(part)) {
							collected = true;
						}
					}
				}

				if (collected) {
					HUDpart->SetColour(1.0f, 1.0f, 1.0f);
				}
			}
		}
	}

	for (size_t i = 0; i < m_playerHealthPool->totalCount(); i++) {
		if (GameObject* obj = m_playerHealthPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<UnderlayTiles*>(obj)) {
				UnderlayTiles* health = static_cast<UnderlayTiles*>(obj);
				float healthLeft = PlayerState::GetInstance().GetHealth();

				float reverseIndex = (i + 1) * 10.0f;

				if (healthLeft < reverseIndex) health->SetColour(1.0f, 0.0f, 0.0f);
				else health->SetColour(1.0f, 1.0f, 1.0f);

				health->Process(deltaTime);
			}
		}
	}

	for (size_t i = 0; i < m_playerLivesPool->totalCount(); i++) {
		if (GameObject* obj = m_playerLivesPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<Life*>(obj)) {
				Life* life = static_cast<Life*>(obj);
				size_t livesLeft = PlayerState::GetInstance().GetLives();

				size_t reverseIndex = m_playerLivesPool->totalCount() - 1 - i;

				if (livesLeft <= reverseIndex) life->SetColour(0.0f, 0.0f, 0.0f);
				else life->SetColour(1.0f, 1.0f, 1.0f);

				life->Process(deltaTime);
			}
		}
	}

	if (GameObject* obj = m_weaponPool->getObjectAtIndex(m_currentWeapon)) {
		if (obj && dynamic_cast<Weapon*>(obj)) {
			Weapon* weapon = static_cast<Weapon*>(obj);
			weapon->Process(deltaTime);

			weapon->Position().y = m_weaponPos.y;
			weapon->Position().x = m_weaponPos.x - weapon->GetOffset();
		}
	}
}

void HUDParser::Draw(Renderer& renderer)
{
	for (size_t i = 0; i < m_HUDTiles->totalCount(); i++) {
		if (GameObject* obj = m_HUDTiles->getObjectAtIndex(i)) {
			if (obj && obj->isActive()) {
				UnderlayTiles* tile = static_cast<UnderlayTiles*>(obj);
				tile->Draw(renderer);
			}
		}
	}

	for (size_t i = 0; i < m_playerHealthPool->totalCount(); i++) {
		if (GameObject* obj = m_playerHealthPool->getObjectAtIndex(i)) {
			if (obj && obj->isActive()) {
				UnderlayTiles* tile = static_cast<UnderlayTiles*>(obj);
				tile->Draw(renderer);
			}
		}
	}

	for (size_t i = 0; i < m_playerLivesPool->totalCount(); i++) {
		if (GameObject* obj = m_playerLivesPool->getObjectAtIndex(i)) {
			if (obj && obj->isActive()) {
				Life* life = static_cast<Life*>(obj);
				life->Draw(renderer);
			}
		}
	}

	for (size_t i = 0; i < m_itemPool->totalCount(); i++) {
		if (GameObject* obj = m_itemPool->getObjectAtIndex(i)) {
			if (obj && obj->isActive()) {
				HUDShipPart* part = static_cast<HUDShipPart*>(obj);
				part->Draw(renderer);
			}
		}
	}

	if (GameObject* obj = m_weaponPool->getObjectAtIndex(m_currentWeapon)) {
		if (obj && obj->isActive()) {
			Weapon* weapon = static_cast<Weapon*>(obj);
			weapon->Draw(renderer);
		}
	}
}

void HUDParser::DebugDraw()
{
}

void HUDParser::SetWeaponHUD(int num)
{
	m_currentWeapon = num;
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

	float levelPixelWidth = (levelWidth * m_tileSize);
	float levelPixelHeight = (levelHeight * m_tileSize);

	screenOffsetX = (m_tileSize / 2.0f) + (m_tileSize / 2); //center tile + offset
	screenOffsetXR = screenWidth - levelPixelWidth; // right aligned
	screenOffsetY = (m_tileSize / 2.0f) + m_tileSize / 2;
	screenOffsetYR = screenHeight - levelPixelHeight; // bottom aligned
	screenOffsetCenter = (screenWidth / 2.0f) - (levelPixelWidth / 2.0f); //center of x axis

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
		if (GameObject* obj = m_HUDTiles->getObject()) {
			UnderlayTiles* tile = dynamic_cast<UnderlayTiles*>(obj);

			if (tile) {
				string filepath = "..\\assets\\hud_corner.png";

				if (!tile->initialise(renderer, filepath.c_str())) {
					return false;
				}

				tile->Position().x = (x * m_tileSize) + screenOffsetXR;
				tile->Position().y = (y * m_tileSize) + screenOffsetY;

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
		if (GameObject* obj = m_HUDTiles->getObject()) {
			UnderlayTiles* tile = dynamic_cast<UnderlayTiles*>(obj);

			if (tile) {
				string filepath = "..\\assets\\hud_middle.png";

				if (!tile->initialise(renderer, filepath.c_str())) {
					return false;
				}

				tile->Position().x = (x * m_tileSize) + screenOffsetXR;
				tile->Position().y = (y * m_tileSize) + screenOffsetY;

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

	case 'I':
	case 'Z':
	case 'Y':
		if (GameObject* obj = m_HUDTiles->getObject()) {
			UnderlayTiles* tile = dynamic_cast<UnderlayTiles*>(obj);

			if (tile) {
				string filepath = "..\\assets\\hud_center.png";

				if (!tile->initialise(renderer, filepath.c_str())) {
					return false;
				}

				tile->Position().y = (y * m_tileSize) + screenOffsetY;

				if (tileType == 'Y') {
					tile->Position().x = (x * m_tileSize) + screenOffsetXR;
					m_weaponPos = tile->Position();
				}
				else if (tileType == 'I') {
					tile->Position().x = (x * m_tileSize) + screenOffsetX;
					m_itemsStartPos = tile->Position();
				}
				else {
					tile->Position().x = (x * m_tileSize) + screenOffsetX;
				}

				tile->SetActive(true);
			}
		}
		return true;

	case 'J':
	case 'K':
		if (GameObject* obj = m_HUDTiles->getObject()) {
			UnderlayTiles* tile = dynamic_cast<UnderlayTiles*>(obj);

			if (tile) {
				string filepath = "..\\assets\\hud_player_corner_left.png";

				if (!tile->initialise(renderer, filepath.c_str())) {
					return false;
				}

				tile->Position().x = (x * m_tileSize) + screenOffsetX;
				tile->Position().y = (y * m_tileSize) + screenOffsetY;

				if (tileType == 'K') {
					tile->SetRotation(180.0f);
				}

				tile->SetActive(true);
			}
		}
		return true;

	case 'L':
	case 'M':
		if (GameObject* obj = m_HUDTiles->getObject()) {
			UnderlayTiles* tile = dynamic_cast<UnderlayTiles*>(obj);

			if (tile) {
				string filepath = "..\\assets\\hud_player_corner_right.png";

				if (!tile->initialise(renderer, filepath.c_str())) {
					return false;
				}

				tile->Position().x = (x * m_tileSize) + screenOffsetX;
				tile->Position().y = (y * m_tileSize) + screenOffsetY;

				if (tileType == 'M') {
					tile->SetRotation(180.0f);
				}

				tile->SetActive(true);
			}
		}
		return true;

	case 'N':
	case 'O':
		if (GameObject* obj = m_HUDTiles->getObject()) {
			UnderlayTiles* tile = dynamic_cast<UnderlayTiles*>(obj);

			if (tile) {
				string filepath = "..\\assets\\hud_player_side.png";

				if (!tile->initialise(renderer, filepath.c_str())) {
					return false;
				}

				tile->Position().x = (x * m_tileSize) + screenOffsetX;
				tile->Position().y = (y * m_tileSize) + screenOffsetY;

				if (tileType == 'O') {
					tile->SetRotation(180.0f);
				}

				tile->SetActive(true);
			}
		}
		return true;

	case 'P':
	case 'Q':
		if (GameObject* obj = m_HUDTiles->getObject()) {
			UnderlayTiles* tile = dynamic_cast<UnderlayTiles*>(obj);

			if (tile) {
				string filepath = "..\\assets\\hud_player_middle.png";

				if (!tile->initialise(renderer, filepath.c_str())) {
					return false;
				}

				tile->Position().x = (x * m_tileSize) + screenOffsetX;
				tile->Position().y = (y * m_tileSize) + screenOffsetY;

				if (tileType == 'Q') {
					tile->SetRotation(180.0f);
				}

				tile->SetActive(true);
			}
		}
		return true;

	case 'R':
	case 'S':
	case 'T':
	case 'U':
		if (GameObject* obj = m_HUDTiles->getObject()) {
			UnderlayTiles* tile = dynamic_cast<UnderlayTiles*>(obj);

			if (tile) {
				string filepath = "..\\assets\\hud_life_corner.png";

				if (!tile->initialise(renderer, filepath.c_str())) {
					return false;
				}

				tile->Position().x = (x * m_tileSize) + screenOffsetCenter;
				tile->Position().y = (y * m_tileSize) + screenOffsetYR;

				switch (tileType) {
				case 'S':
					tile->SetRotation(90.0f);
					break;
				case 'T':
					tile->SetRotation(180.0f);
					break;
				case 'U':
					tile->SetRotation(-90.0f);
					break;
				}

				tile->SetActive(true);
			}
		}
		return true;

	case 'V':
	case 'W':
	case 'X':
	case '0':
		if (GameObject* obj = m_HUDTiles->getObject()) {
			UnderlayTiles* tile = dynamic_cast<UnderlayTiles*>(obj);

			if (tile) {
				string filepath = "..\\assets\\hud_life_middle.png";

				if (!tile->initialise(renderer, filepath.c_str())) {
					return false;
				}

				tile->Position().x = (x * m_tileSize) + screenOffsetCenter;
				tile->Position().y = (y * m_tileSize) + screenOffsetYR;

				switch (tileType) {
				case 'W':
					tile->SetRotation(90.0f);
					m_livesStartPos = tile->Position();
					m_livesStartPos.x += tile->GetSpriteWidth();
					break;
				case 'X':
					tile->SetRotation(180.0f);
					break;
				case '0':
					tile->SetRotation(-90.0f);
					break;
				}

				tile->SetActive(true);
			}
		}
		return true;

	case '1':
	case '2':
	case '3':
	case '4':
		if (GameObject* obj = m_HUDTiles->getObject()) {
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

				tile->Position().x = (x * m_tileSize) + screenOffsetX + 25.0f;
				tile->Position().y = (y * m_tileSize) + screenOffsetY + 25.0f;

				tile->SetActive(true);
			}
		}
		return true;

	case '+':
	case '|':
	case '\\':
		if (GameObject* obj = m_playerHealthPool->getObject()) {
			UnderlayTiles* tile = dynamic_cast<UnderlayTiles*>(obj);

			if (tile) {
				string filepath;

				if (tileType == '+') {
					filepath = "..\\assets\\health_middle.png";
				}
				else if (tileType == '|') {
					filepath = "..\\assets\\health_start.png";
				}
				else if (tileType == '\\') {
					filepath = "..\\assets\\health_end.png";
				}

				if (!tile->initialise(renderer, filepath.c_str())) {
					return false;
				}

				tile->Position().x = ((x * m_tileSize) + screenOffsetX) - (m_tileSize / 2.0f);
				tile->Position().y = (y * m_tileSize) + screenOffsetY;

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

bool HUDParser::WeaponsInitialised(Renderer& renderer)
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

			default:
				filepath += "basic_knife.png";
				break;
			}

			weapon->initialise(renderer, filepath.c_str());
			weapon->SetWeapon(true);
			weapon->SetSize(3.0f);

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

			default:
				weapon->SetWeaponType('M');
				break;
			}
			
			//offset the weapons x to center it within the square
			weapon->SetOffset(weapon->GetSpriteWidth() * 0.25f);
		}
	}

	m_currentWeapon = 0;

	return true;
}

bool HUDParser::LivesInitialised(Renderer& renderer)
{
	for (size_t i = 0; i < m_playerLivesPool->totalCount(); i++) {
		if (GameObject* obj = m_playerLivesPool->getObjectAtIndex(i)) {
			Life* life = static_cast<Life*>(obj);
			life->initialise(renderer);
			
			int multiplier = i * 4;

			life->Position() = m_livesStartPos;

			if (i > 0) life->Position().x -= life->GetSpriteWidth() * multiplier;

		}
	}

	return true;
}

void HUDParser::PositionItems(Renderer& renderer)
{
	for (size_t i = 0; i < m_itemPool->totalCount(); i++) {
		if (GameObject* obj = m_itemPool->getObjectAtIndex(i)) {
			HUDShipPart* part = static_cast<HUDShipPart*>(obj);
			part->Initialise(renderer);
			part->SetActive(true);

			part->Position() = m_itemsStartPos;
			part->Position().y += itemOffset;

			itemOffset += part->GetSpriteWidth();
		}
	}
}
