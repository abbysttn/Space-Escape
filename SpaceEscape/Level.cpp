#include "Level.h"

#include "Renderer.h"
#include "DDLevelLoad.h"
#include "gameobjectpool.h"
#include "logmanager.h"
#include "inputsystem.h"
#include "xboxcontroller.h"
#include "quadtree.h"
#include "soundsystem.h"

#include "imgui/imgui.h"
#include "inlinehelpers.h"

#include "HUDParser.h"
#include "TileParser.h"

#include "water.h"
#include "player.h"
#include "weapon.h"
#include "bullet.h"
#include "enemy.h"
#include "enemyspawner.h"

Level::Level(string levelType, char levelDifficulty, int levelNumber, char gameDifficulty) : m_playerSize(48.0f), m_soundSystem(0), m_playerPrevPosition(0, 0), m_playerPosition(0, 0), m_currentPlayer(0), m_playerPool(nullptr),
m_waterPool(nullptr), m_tileSize(48.0f), m_hudParser(0), m_tileParser(0), m_weaponPool(nullptr), m_bulletPool(nullptr), m_spawnerPool(nullptr), 
m_enemyPool(nullptr), m_playerPushed(false), m_playerAlive(true), m_gameOver(false), m_levelDone(false), m_invulnerability(false), m_levelType(levelType), m_levelDifficulty(levelDifficulty),
m_levelNumber(levelNumber), m_gameDifficulty(gameDifficulty) {}

Level::~Level()
{
	delete m_playerPool;
	delete m_waterPool;
	delete m_hudParser;
	delete m_bulletPool;
	delete m_weaponPool;
	delete m_spawnerPool;
	delete m_enemyPool;

	SoundSystem::getInstance().close();
}

bool Level::Initialise(Renderer& renderer)
{
	m_cooldownTime = 0.2f;
	m_currentCooldown = 0.0f;
	m_enemySpawnTimer = 0.0f;
	m_enemySpawnTime = 8.0f;
	m_maxEnemies = 20;
	m_currentEnemies = 0;

	m_hudParser = new HUDParser();
	m_tileParser = new TileParser(m_levelType, m_levelNumber);

	m_playerPool = new GameObjectPool(Player(), 4);
	m_weaponPool = new GameObjectPool(Weapon(), 6);
	m_bulletPool = new GameObjectPool(Bullet(), 20);
	m_enemyPool = new GameObjectPool(Enemy(), 20);

	m_tileParser->Initialise(renderer);
	m_hudParser->Initialise(renderer);

	m_playerPosition = m_tileParser->GetPlayerStartPosition();
	m_waterPool = m_tileParser->GetWaterPool();
	m_spawnerPool = m_tileParser->GetSpawnerPool();

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

	if (!EnemiesInitialised(renderer)) {
		LogManager::GetInstance().Log("Unable to Initialise Enemies!");
		return false;
	}
	
	LogManager::GetInstance().Log("Initialised all Sprites!");

	m_boundaryCollisionTree = make_unique<QuadTree>(Box(0.0f, 0.0f, (float)renderer.GetWidth(), (float)renderer.GetHeight()));
	m_enemyCollisionTree = make_unique<QuadTree>(Box(0.0f, 0.0f, (float)renderer.GetWidth(), (float)renderer.GetHeight()));

	return true;

}

void Level::Process(float deltaTime, InputSystem& inputSystem)
{
	m_boundaryCollisionTree->clear();
	m_enemyCollisionTree->clear();

	//timers

	if (m_currentCooldown > 0.0f) {
		m_currentCooldown -= deltaTime;
	}

	m_enemySpawnTimer += deltaTime;

	if (m_enemySpawnTimer >= m_enemySpawnTime && m_currentEnemies < m_maxEnemies) {
		SpawnEnemy();
		m_enemySpawnTimer = 0.0f;
	}

	//

	m_tileParser->Process(deltaTime, inputSystem);

	AddWaterCollision();

	PlayerMovement(inputSystem, m_currentPlayer, deltaTime);

	if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
		if (obj && dynamic_cast<Player*>(obj)) {
			Player* player = static_cast<Player*>(obj);
			if (player->isActive()) {

				bool colliding = PlayerColliding(player);
				if (!colliding) {
					player->Process(deltaTime);
				}

				m_playerPushed = player->IsPushedBack();

				if (!player->IsRunning()) {
					m_currentPlayer = 0;
				}
			}
			else {
				m_playerAlive = false;
				GameOver();
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

	for (size_t i = 0; i < m_enemyPool->totalCount(); i++) {
		if (GameObject* obj = m_enemyPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<Enemy*>(obj)) {
				Enemy* enemy = static_cast<Enemy*>(obj);

				float enemySize = (float)enemy->GetSpriteWidth();
				Box enemyRange(
					enemy->Position().x,
					enemy->Position().y,
					enemySize,
					enemySize
				);

				m_enemyCollisionTree->insert(enemy, enemyRange);

				//enemy->GetUpdatedPosition(deltaTime, m_playerPrevPosition);

				bool colliding = EnemyColliding(enemy);
				if (!colliding) {
					enemy->Process(deltaTime, m_playerPrevPosition);
				}
			}
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

	DoDamage();

	m_hudParser->Process(deltaTime, inputSystem);
}

void Level::Draw(Renderer& renderer)
{
	renderer.SetClearColour(71, 171, 169);
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

	for (size_t i = 0; i < m_enemyPool->totalCount(); i++) {
		if (GameObject* obj = m_enemyPool->getObjectAtIndex(i)) {
			if (obj && obj->isActive()) {
				Enemy* enemy = static_cast<Enemy*>(obj);
				enemy->Draw(renderer);
			}
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

	ImGui::Checkbox("Invulnerability", &m_invulnerability);
	if (m_invulnerability)
	{
		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = static_cast<Player*>(obj);
			player->SetDamageTaken(false);
		}
	}
	else {
		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = static_cast<Player*>(obj);
			player->SetDamageTaken(true);
		}
	}
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

				player->SetActive(true);
				player->Position().x = m_playerPosition.x;
				player->Position().y = m_playerPosition.y;
				player->SetLoop(true);
				player->SetFrameTime(0.05f);
				break;

			case 2:
				if (!player->Initialise(renderer, "..\\assets\\run_left.png")) {
					return false;
				}

				player->SetActive(true);
				player->Position().x = m_playerPosition.x;
				player->Position().y = m_playerPosition.y;
				player->SetLoop(true);
				player->SetFrameTime(0.05f);
				break;

			case 3: // use
				if (!player->Initialise(renderer, "..\\assets\\use.png")) {
					return false;
				}

				player->SetActive(true);
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

	if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
		Player* player = static_cast<Player*>(obj);
		if (m_playerPushed) {
			Vector2 pushbackPos = player->GetUpdatedPushPosition(deltaTime);

			player->Position() = pushbackPos;
			bool collision = PlayerColliding(player);

			if (collision) {
				player->SetPushedBack(false);
				m_playerPushed = false;
			}
			else {
				m_playerPosition = pushbackPos;
			}

			return;
		}
	}

	Vector2 updatedPos = m_playerPosition;
	XboxController* controller = inputSystem.GetController(0);

	if (inputSystem.GetKeyState(SDL_SCANCODE_RIGHT) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_RIGHT) == BS_PRESSED
		|| inputSystem.GetKeyState(SDL_SCANCODE_D) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_D) == BS_PRESSED) {
		m_currentPlayer = 1;
		m_currentDirection = 'R';
		SwitchDirection(m_currentDirection);
		updatedPos.x += 80.0f * deltaTime;
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_LEFT) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_LEFT) == BS_PRESSED
		|| inputSystem.GetKeyState(SDL_SCANCODE_A) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_A) == BS_PRESSED) {
		m_currentPlayer = 2;
		m_currentDirection = 'L';
		SwitchDirection(m_currentDirection);
		updatedPos.x -= 80.0f * deltaTime;
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_UP) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_UP) == BS_PRESSED
		|| inputSystem.GetKeyState(SDL_SCANCODE_W) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_W) == BS_PRESSED) {

		if (m_currentPlayer != 2) {
			m_currentPlayer = 1;
		}

		updatedPos.y -= 80.0f * deltaTime;
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_DOWN) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_DOWN) == BS_PRESSED
		|| inputSystem.GetKeyState(SDL_SCANCODE_S) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_S) == BS_PRESSED) {

		if (m_currentPlayer != 1) {
			m_currentPlayer = 2;
		}

		updatedPos.y += 80.0f * deltaTime;
	}

	//detect any collision from the potential movement:

	bool collision = false;

	if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
		Player* player = static_cast<Player*>(obj);
		collision = PlayerColliding(player);
	}

	if (!collision) {
		m_playerPosition = updatedPos;
	}
	else {
	}

	if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
		Player* player = static_cast<Player*>(obj);
		player->Position() = m_playerPosition;
	}

	if (inputSystem.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED) { // weapon swing
		m_currentPlayer = 3;
		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = static_cast<Player*>(obj);
			player->SetRunning();
			player->Position() = m_playerPosition;
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
		inputSystem.GetKeyState(SDL_SCANCODE_UP) == BS_RELEASED || inputSystem.GetKeyState(SDL_SCANCODE_DOWN) == BS_RELEASED
		|| inputSystem.GetKeyState(SDL_SCANCODE_A) == BS_RELEASED || inputSystem.GetKeyState(SDL_SCANCODE_D) == BS_RELEASED
		|| inputSystem.GetKeyState(SDL_SCANCODE_W) == BS_RELEASED || inputSystem.GetKeyState(SDL_SCANCODE_S) == BS_RELEASED) {
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
		}
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_E) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_E) == BS_PRESSED) {
		m_currentPlayer = 3;

		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = static_cast<Player*>(obj);
			player->SetRunning();
			player->Position() = m_playerPosition;
		}
	}
}

bool Level::IsColliding(const Box& box, Water* water)
{
	if (!water) return false;

	float wX = water->Position().x;
	float wY = water->Position().y;
	float wW = (float)water->GetSpriteWidth();

	return box.x < wX + wW &&
		box.x + box.width > wX &&
		box.y < wY + wW &&
		box.y + box.height > wY;
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
				weapon->SetWeaponDamage(0);
				break;
			case 1:
				weapon->SetWeaponType('M');
				weapon->SetWeaponDamage(1);
				break;
			case 2:
				weapon->SetWeaponType('G');
				weapon->SetWeaponDamage(2);
				break;
			case 3:
				weapon->SetWeaponType('G');
				weapon->SetWeaponDamage(3);
				break;
			case 4:
				weapon->SetWeaponType('G');
				weapon->SetWeaponDamage(2);
				break;
			case 5:
				weapon->SetWeaponType('G');
				weapon->SetWeaponDamage(3);
				break;

			default:
				weapon->SetWeaponType('M');
				weapon->SetWeaponDamage(0);
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

bool Level::EnemiesInitialised(Renderer& renderer)
{
	for (size_t i = 0; i < m_enemyPool->totalCount(); i++) {
		if (GameObject* obj = m_enemyPool->getObjectAtIndex(i)) {
			Enemy* enemy = static_cast<Enemy*>(obj);

			string enemyPath;

			switch (m_levelDifficulty) {
			case 'E':
				enemyPath = "easy";
				break;
			case 'M':
				enemyPath = "medium";
				break;
			case 'H':
				enemyPath = "hard";
				break;
			default:
				enemyPath = "easy";
				break;
			}

			string filepath = "..\\assets\\alien_" + enemyPath + ".png";

			enemy->Initialise(renderer, filepath.c_str());
			enemy->SetActive(false);
			enemy->SetAttackDamage(m_levelDifficulty, m_gameDifficulty);
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

					m_boundaryCollisionTree->insert(water, waterRange);
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
		case 0:
			if (GameObject* obj = m_weaponPool->getObjectAtIndex(m_currentWeapon)) {
				Weapon* weapon = static_cast<Weapon*>(obj);
				weapon->SetRotation(180.0f);
			}
			break;

		case 1:
			if (GameObject* obj = m_weaponPool->getObjectAtIndex(m_currentWeapon)) {
				Weapon* weapon = static_cast<Weapon*>(obj);
				weapon->SetRotation(180.0f);
			}
			break;

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
		case 0:
			if (GameObject* obj = m_weaponPool->getObjectAtIndex(m_currentWeapon)) {
				Weapon* weapon = static_cast<Weapon*>(obj);
				weapon->SetRotation(0.0f);
			}
			break;

		case 1:
			if (GameObject* obj = m_weaponPool->getObjectAtIndex(m_currentWeapon)) {
				Weapon* weapon = static_cast<Weapon*>(obj);
				weapon->SetRotation(0.0f);
			}
			break;

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

void Level::SpawnEnemy()
{
	if (!m_enemyPool->hasAvailableObjects()) {
		LogManager::GetInstance().Log("No available enemies in pool!");
		return;
	}

	if (GameObject* obj = m_enemyPool->getObject()) {
		Enemy* enemy = dynamic_cast<Enemy*>(obj);
		if (enemy) {

			int spawner = GetRandom(0, m_spawnerPool->totalCount() - 1);

			if (GameObject* spawnObj = m_spawnerPool->getObjectAtIndex(spawner)) {
				if (EnemySpawner* spawner = dynamic_cast<EnemySpawner*>(spawnObj)) {
					enemy->Position() = spawner->Position();
				}
			}

			enemy->SetActive(true);
		}
	}

	m_currentEnemies++;
}

bool Level::EnemyColliding(Enemy* enemy)
{
	bool collision = false;

	Box enemyBox(
		enemy->Position().x, enemy->Position().y,
		enemy->GetSpriteWidth(), enemy->GetSpriteWidth());

	Box waterBox;

	auto potentialCollisions = m_boundaryCollisionTree->queryRange(enemyBox);

	for (auto* obj : potentialCollisions) {
		Water* water = dynamic_cast<Water*>(obj);
		if (water && IsColliding(enemyBox, water)) {
			collision = true;
			waterBox = { water->Position().x, water->Position().y,
				(float)water->GetSpriteWidth(), (float)water->GetSpriteWidth() };
			break;
		}
	}

	if (collision) {
		HandleEnemyCollision(waterBox, enemy);
	}

	return collision;
}

void Level::HandleEnemyCollision(const Box& collision, Enemy* enemy)
{
	Vector2 center(enemy->Position().x + enemy->GetSpriteWidth() / 2.0f, enemy->Position().y + enemy->GetSpriteWidth() / 2.0f);
	Vector2 waterCenter(collision.x + collision.width / 2.0f, collision.y + collision.height / 2.0f);

	Vector2 direction(center.x - waterCenter.x, center.y - waterCenter.y);

	float length = sqrt(direction.x * direction.x + direction.y * direction.y);

	if (length != 0) {
		direction.x /= length;
		direction.y /= length;

		enemy->Position().x += direction.x * 5.0f;
		enemy->Position().y += direction.y * 5.0f;
	}

	enemy->ResetWander(direction);
}

bool Level::PlayerColliding(Player* player)
{
	bool collision = false;

	Box playerBox(
		player->Position().x, player->Position().y,
		player->GetSpriteWidth(), player->GetSpriteWidth());

	Box waterBox;

	auto potentialCollisions = m_boundaryCollisionTree->queryRange(playerBox);

	for (auto* obj : potentialCollisions) {
		Water* water = dynamic_cast<Water*>(obj);
		if (water && IsColliding(playerBox, water)) {
			collision = true;
			waterBox = { water->Position().x, water->Position().y,
				(float)water->GetSpriteWidth(), (float)water->GetSpriteWidth() };
			break;
		}
	}

	if (collision) {
		HandlePlayerCollision(waterBox, player);
	}

	return collision;
}

void Level::HandlePlayerCollision(const Box& collision, Player* player)
{
	Vector2 center(player->Position().x + player->GetSpriteWidth() / 2.0f,
		player->Position().y + player->GetSpriteWidth() / 2.0f);
	Vector2 waterCenter(collision.x + collision.width / 2.0f,
		collision.y + collision.height / 2.0f);

	Vector2 direction(center.x - waterCenter.x, center.y - waterCenter.y);

	float length = sqrt(direction.x * direction.x + direction.y * direction.y);

	if (length != 0) {
		direction.x /= length;
		direction.y /= length;

		// Push the player back from the water
		player->Position().x += direction.x * 5.0f;
		player->Position().y += direction.y * 5.0f;

		m_playerPrevPosition = m_playerPosition;
		m_playerPosition = player->Position();
	}
}

bool Level::DamageCollision(Enemy* enemy, const Box& collision)
{
	bool colliding;

	if (!enemy) {
		return false;
	}

	float eX = enemy->Position().x;
	float eY = enemy->Position().y;
	float eW = (float)enemy->GetSpriteWidth();

	colliding = eX < collision.x + collision.width &&
		eX + eW > collision.x &&
		eY < collision.y + collision.width &&
		eY + eW > collision.y;

	return colliding;
}

void Level::DoDamage()
{
	Box playerBox(m_playerPosition.x, m_playerPosition.y, m_playerSize, m_playerSize);

	char weaponType = 'M';
	bool isSwinging = false;
	float weaponDamage = 0.0f;

	if (GameObject* obj = m_weaponPool->getObjectAtIndex(m_currentWeapon)) {
		Weapon* weapon = dynamic_cast<Weapon*>(obj);
		if (weapon) {
			weaponType = weapon->GetWeaponType();
			isSwinging = weapon->IsSwinging();
			weaponDamage = weapon->GetWeaponDamage();
		}
	}

	switch (weaponType) {
	case 'M': {
		auto potentialCollisions = m_enemyCollisionTree->queryRange(playerBox);

		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = dynamic_cast<Player*>(obj);

			for (auto* obj : potentialCollisions) {
				Enemy* enemy = dynamic_cast<Enemy*>(obj);
				if (enemy->isActive() && DamageCollision(enemy, playerBox)) {
					Vector2 pushDirection(enemy->Position().x - player->Position().x,
						enemy->Position().y - player->Position().y);

					if (isSwinging) {
						enemy->ApplyPushBack(pushDirection);
						enemy->AddDamage(weaponDamage);
					}
					else {
						Vector2 pushDirectionPlayer(player->Position().x - enemy->Position().x,
							player->Position().y - enemy->Position().y);
						player->ApplyPushBack(pushDirectionPlayer);
						player->AddDamage(enemy->GetDamageDealt());
						enemy->ApplyPushBack(pushDirection);
					}
					break;
				}
			}
			break;
		}
	}

	case 'G': {
		for (size_t i = 0; i < m_bulletPool->totalCount(); i++) {
			if (GameObject* obj = m_bulletPool->getObjectAtIndex(i)) {
				if (Bullet* bullet = static_cast<Bullet*>(obj)) {
					if (bullet->isActive()) {
						float bulletSize = bullet->GetSpriteWidth();
						Box bulletRange(
							bullet->Position().x - bulletSize / 2,
							bullet->Position().y - bulletSize / 2,
							bulletSize,
							bulletSize
						);

						auto potentialCollisions = m_enemyCollisionTree->queryRange(bulletRange);

						for (auto* obj : potentialCollisions) {
							Enemy* enemy = dynamic_cast<Enemy*>(obj);

							if (enemy->isActive() && DamageCollision(enemy, bulletRange)) {
								bullet->SetActive(false);
								Vector2 pushDirection(enemy->Position().x - bullet->Position().x,
									enemy->Position().y - bullet->Position().y);

								enemy->ApplyPushBack(pushDirection);
								enemy->AddDamage(weaponDamage);

								m_bulletPool->release(bullet);
								break;
							}
						}

						if (!bullet->isActive()) {
							m_bulletPool->release(bullet);
						}
					}
				}
			}
		}

		auto potentialPlayerCollisions = m_enemyCollisionTree->queryRange(playerBox);

		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = dynamic_cast<Player*>(obj);

			for (auto* obj : potentialPlayerCollisions) {
				Enemy* enemy = dynamic_cast<Enemy*>(obj);
				if (enemy->isActive() && DamageCollision(enemy, playerBox)) {
					Vector2 pushDirection(enemy->Position().x - player->Position().x,
						enemy->Position().y - player->Position().y);
					Vector2 pushDirectionPlayer(player->Position().x - enemy->Position().x,
						player->Position().y - enemy->Position().y);
					player->ApplyPushBack(pushDirectionPlayer);
					player->AddDamage(enemy->GetDamageDealt());
					enemy->ApplyPushBack(pushDirection);

					break;
				}
			}
			break;
		}
		break;
	}

	default:
		LogManager::GetInstance().Log("No weapon type found");
		break;
	}
}

void Level::GameOver()
{
	for (size_t i = 0; i < m_playerPool->totalCount(); i++) {
		if (GameObject* obj = m_playerPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<Player*>(obj)) {
				Player* player = static_cast<Player*>(obj);
				player->SetActive(false);
			}
		}
	}

	for (size_t i = 0; i < m_weaponPool->totalCount(); i++) {
		if (GameObject* obj = m_weaponPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<Weapon*>(obj)) {
				Weapon* weapon = static_cast<Weapon*>(obj);
				weapon->SetWeapon(false);
			}
		}
	}

	m_gameOver = true;
}

void Level::NextLevel()
{
	// if rift vial collected and used
	m_levelDone = true;
}

