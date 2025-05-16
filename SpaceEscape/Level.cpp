#include "Level.h"

#include "Renderer.h"
#include "DDLevelLoad.h"
#include "gameobjectpool.h"
#include "logmanager.h"
#include "inputsystem.h"
#include "xboxcontroller.h"
#include "quadtree.h"
#include "playerstate.h"

#include "imgui/imgui.h"
#include "inlinehelpers.h"
#include "collisionhelper.h"
#include "particleemitter.h"
#include "soundsystem.h"

#include "HUDParser.h"
#include "TileParser.h"
#include "difficultybanner.h"

#include "water.h"
#include "player.h"
#include "weapon.h"
#include "bullet.h"
#include "enemy.h"
#include "enemyspawner.h"
#include "riftvial.h"
#include "itemmanager.h"
#include "bossenemy.h"
#include "dropableweapon.h"

#include "pausescene.h"
#include "weaponstate.h"

Level::Level(string levelType, char levelDifficulty, int levelMap, char gameDifficulty, int levelNumber, int planetEffect) : m_playerSize(48.0f), m_playerPrevPosition(0, 0), m_playerPosition(0, 0), m_currentPlayer(0), m_playerPool(nullptr),
m_waterPool(nullptr), m_tileSize(48.0f), m_hudParser(0), m_tileParser(0), m_weaponPool(nullptr), m_bulletPool(nullptr), m_spawnerPool(nullptr), 
m_enemyPool(nullptr), m_playerPushed(false), m_playerAlive(true), m_gameOver(false), m_levelDone(false), m_invulnerability(false), m_levelType(levelType), m_levelDifficulty(levelDifficulty),
m_levelNumber(levelNumber), m_levelMap(levelMap), m_gameDifficulty(gameDifficulty), m_riftVial(0), m_boss(0), m_deathParticles(0), m_particleTime(0.0f), m_particleMaxTime(0.5f),
m_particleSpawned(false), m_sounds(0), m_banner(0), m_planetEffect(planetEffect), m_paused(false), m_pause(0), m_upgradePool(nullptr), m_bossSpawned(false), m_spawnEnemies(true) {}

Level::~Level()
{
	delete m_playerPool;
	delete m_waterPool;
	delete m_hudParser;
	delete m_banner;
	delete m_bulletPool;
	delete m_weaponPool;
	delete m_spawnerPool;
	delete m_enemyPool;

	delete m_pause;

	delete m_riftVial;
	delete m_boss;
	delete m_deathParticles;
}

bool Level::Initialise(Renderer& renderer)
{
	SetRenderColour(m_levelType);

	m_pause = new PauseScene();
	m_pause->Initialise(renderer);

	m_cooldownTime = 0.2f;
	m_currentCooldown = 0.0f;
	m_enemySpawnTimer = 0.0f;
	m_enemySpawnTime = 4.0f;
	m_maxEnemies = 3;
	m_currentEnemies = 0;

	m_sounds = new SoundSystem();
	m_sounds->initialise();
	m_sounds->loadSound("hit", "..\\assets\\hit.wav", false);
	m_sounds->loadSound("collect", "..\\assets\\collect.wav", false);
	m_sounds->loadSound("shoot", "..\\assets\\shoot.wav", false);

	m_banner = new DifficultyBanner(m_planetEffect, m_levelDifficulty, m_levelNumber);
	m_banner->Initialise(renderer);
	
	m_itemPool = ItemManager::GetInstance().GetPool();

	m_centerPos = { renderer.GetWidth() / 2.0f, renderer.GetHeight() / 2.0f };

	m_hudParser = new HUDParser();
	m_tileParser = new TileParser(m_levelType, m_levelMap);

	m_playerPool = new GameObjectPool(Player(), 4);
	m_weaponPool = new GameObjectPool(Weapon(), 6);
	m_bulletPool = new GameObjectPool(Bullet(), 20);
	m_enemyPool = new GameObjectPool(Enemy(), 20);
	m_upgradePool = new GameObjectPool(DropableWeapon(), 4);
	m_boss = new BossEnemy();

	m_riftVial = new RiftVial();
	m_riftVial->Initialise(renderer);

	m_deathParticles = new ParticleEmitter();
	
	if (!ParticlesInitialised(renderer)) {
		LogManager::GetInstance().Log("Unable to Initialise Particles!");
		return false;
	}

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

	if (m_levelNumber != 5) {
		if (!EnemiesInitialised(renderer)) {
			LogManager::GetInstance().Log("Unable to Initialise Enemies!");
			return false;
		}
	}
	else {
		m_boss->Initialise(renderer, m_levelNumber);
		m_boss->SetGameDifficulty(m_gameDifficulty);
		m_boss->SetActive(true);
	}
	
	LogManager::GetInstance().Log("Initialised all Sprites!");

	m_boundaryCollisionTree = make_unique<QuadTree>(Box(0.0f, 0.0f, (float)renderer.GetWidth(), (float)renderer.GetHeight()));
	m_enemyCollisionTree = make_unique<QuadTree>(Box(0.0f, 0.0f, (float)renderer.GetWidth(), (float)renderer.GetHeight()));

	return true;

}

void Level::Process(float deltaTime, InputSystem& inputSystem)
{
	m_sounds->update();

	m_boundaryCollisionTree->clear();
	m_enemyCollisionTree->clear();

	if (inputSystem.GetKeyState(SDL_SCANCODE_ESCAPE) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_ESCAPE) == BS_PRESSED) {
		m_paused = true;
	}

	if (m_paused) {
		m_pause->Paused();
		m_pause->Process(deltaTime, inputSystem);
		if (m_pause->Resume()) {
			m_paused = false;
		}
	}
	else {

		m_tileParser->Process(deltaTime, inputSystem);

		AddWaterCollision();

		m_banner->Process(deltaTime, inputSystem);

		if (m_banner->GetStatus()) {
			//timers

			if (m_currentCooldown > 0.0f) {
				m_currentCooldown -= deltaTime;
			}

			if (m_levelNumber != 5) {
				m_enemySpawnTimer += deltaTime;

				if (m_enemySpawnTimer >= m_enemySpawnTime && m_currentEnemies < m_maxEnemies) {
					SpawnEnemy();
					m_enemySpawnTimer = 0.0f;
				}
			}

			if (m_particleTime < m_particleMaxTime) {
				m_particleTime += deltaTime;
			}

			if (m_particleTime >= m_particleMaxTime) {
				m_particleSpawned = false;
				m_deathParticles->Reset();
			}

			//

			if (m_particleSpawned) {
				m_deathParticles->Process(deltaTime);
			}

			PlayerMovement(inputSystem, m_currentPlayer, deltaTime);

			if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
				if (obj && dynamic_cast<Player*>(obj)) {
					Player* player = static_cast<Player*>(obj);
					if (player->isActive()) {

						bool colliding = CheckWaterCollision(player);
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
						m_gameOver = true;
						NextLevel();
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

			for (size_t i = 0; i < m_upgradePool->totalCount(); i++) {
				if (GameObject* obj = m_upgradePool->getObjectAtIndex(i)) {
					if (obj && dynamic_cast<DropableWeapon*>(obj)) {
						DropableWeapon* weapon = static_cast<DropableWeapon*>(obj);
						weapon->Process(deltaTime);
					}
				}
			}

			if (m_levelNumber != 5) {
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

							bool colliding = CheckWaterCollision(enemy);
							if (!colliding) {
								enemy->Process(deltaTime, m_playerPrevPosition);
							}
						}
					}
				}
			}

			if (m_boss->isActive()) {
				float bossSize = (float)m_boss->GetSpriteWidth();
				Box bossRange(
					m_boss->Position().x,
					m_boss->Position().y,
					bossSize,
					bossSize
				);

				m_enemyCollisionTree->insert(m_boss, bossRange);

				bool colliding = CheckWaterCollision(m_boss);
				if (!colliding) {
					m_boss->Process(deltaTime, m_playerPosition);
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

			if (AllEnemiesDefeated() && m_levelNumber != 5) {
				m_riftVial->Drop(m_centerPos);
			}
			else if (m_levelNumber == 5 && !m_boss->isActive()) {
				m_riftVial->Drop(m_centerPos);
			}

			m_riftVial->Process(deltaTime);

			for (size_t i = 0; i < m_itemPool->totalCount(); i++) {
				if (GameObject* obj = m_itemPool->getObjectAtIndex(i)) {
					if (obj && dynamic_cast<ShipPart*>(obj)) {
						ShipPart* part = static_cast<ShipPart*>(obj);
						if (part->isActive()) part->Process(deltaTime);
					}
				}
			}

			m_hudParser->Process(deltaTime, inputSystem);
		}
	}
}

void Level::Draw(Renderer& renderer)
{
	renderer.SetClearColour(m_renderColour1, m_renderColour2, m_renderColour3);
	m_tileParser->Draw(renderer);

	if (!m_banner->GetStatus()) {
		m_banner->Draw(renderer);
	}
	else if (m_banner->GetStatus()) {
		m_deathParticles->Draw(renderer);

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
		
		for (size_t i = 0; i < m_upgradePool->totalCount(); i++) {
			if (GameObject* obj = m_upgradePool->getObjectAtIndex(i)) {
				if (obj && obj->isActive()) {
					DropableWeapon* weapon = static_cast<DropableWeapon*>(obj);
					weapon->Draw(renderer);
				}
			}
		}

		if (m_levelNumber != 5) {
			for (size_t i = 0; i < m_enemyPool->totalCount(); i++) {
				if (GameObject* obj = m_enemyPool->getObjectAtIndex(i)) {
					if (obj && obj->isActive()) {
						Enemy* enemy = static_cast<Enemy*>(obj);
						enemy->Draw(renderer);
					}
				}
			}
		}

		m_boss->Draw(renderer);

		for (size_t i = 0; i < m_bulletPool->totalCount(); i++) {
			if (GameObject* obj = m_bulletPool->getObjectAtIndex(i)) {
				if (obj && obj->isActive()) {
					Bullet* bullet = static_cast<Bullet*>(obj);
					bullet->Draw(renderer);
				}
			}
		}

		if (AllEnemiesDefeated()) {
			m_riftVial->Draw(renderer);
		}

		for (size_t i = 0; i < m_itemPool->totalCount(); i++) {
			if (GameObject* obj = m_itemPool->getObjectAtIndex(i)) {
				if (obj && obj->isActive()) {
					ShipPart* part = static_cast<ShipPart*>(obj);
					if (part->isActive()) part->Draw(renderer);
				}
			}
		}

		m_hudParser->Draw(renderer);
	}

	if (m_paused) {
		m_pause->Draw(renderer);
	}
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

	string effect = GetPlanetEffect(m_planetEffect);

	ImGui::Text("Level Effect - %s", effect.c_str());

	ImGui::Separator();

	ImGui::Text("Player Info:");

	ImGui::Text("Player Coords: %.2f, %.2f", m_playerPosition.x, m_playerPosition.y);

	ImGui::Text("Player Lives: %d lives", PlayerState::GetInstance().GetLives());

	if (ImGui::Button("Add Life")) {
		PlayerState::GetInstance().AddLife();
	}

	ImGui::Text("Player Health: %.1f health", PlayerState::GetInstance().GetHealth());

	if (ImGui::Button("Add Health")) {
		PlayerState::GetInstance().AddHealth();
	}

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

	ImGui::Separator();

	ImGui::Text("Enemy Information:");
	ImGui::SliderInt("Max Enemy Count", &m_maxEnemies, 1, 20);
	if (ImGui::Button("Remove All Enemies")) {
		if (m_levelNumber != 5) {
			for (size_t i = 0; i < m_enemyPool->totalCount(); i++) {
				if (GameObject* obj = m_enemyPool->getObjectAtIndex(i)) {
					if (obj && dynamic_cast<Enemy*>(obj)) {
						Enemy* enemy = static_cast<Enemy*>(obj);
						enemy->SetActive(false);
					}
				}
			}
			m_boss->SetActive(false);
			if (m_itemPool->hasAvailableObjects()) {
				if (GameObject* obj = m_itemPool->getObject()) {
					ShipPart* part = static_cast<ShipPart*>(obj);
					if (part) {
						part->Drop(m_playerPrevPosition);
					}
				}
			}
		}
		else {
			m_boss->SetActive(false);
			if (m_itemPool->hasAvailableObjects()) {
				int offset = -48;
				for (size_t i = 0; i < 3; i++) {
					if (GameObject* obj = m_itemPool->getObject()) {
						ShipPart* part = static_cast<ShipPart*>(obj);
						if (part) {
							Vector2 position = m_boss->Position();
							position.x += offset;
							part->Drop(position);
							offset += 48;
						}
					}
				}
			}
		}
		m_spawnEnemies = false;
		m_enemiesRemoved = true;
	}
	ImGui::Text("Spawn Enemies: %s", m_spawnEnemies ? "ON" : "OFF");
	if (ImGui::Button("Toggle")) {
		m_spawnEnemies = !m_spawnEnemies;
	}
	ImGui::SliderFloat("Enemy spawn rate:", &m_enemySpawnTime, 1.0f, 10.0f);
	ImGui::Separator();

	ImGui::Text("Item Information:");
	ImGui::Text("Collected Items - %d", ItemManager::GetInstance().GetItemCollectedCount());
	if (ImGui::Button("Add Collected Item")) {
		if (GameObject* obj = m_itemPool->getObject()) {
			if (obj && dynamic_cast<ShipPart*>(obj)) {
				ShipPart* part = static_cast<ShipPart*>(obj);
				if (!part->IsCollected()) {
					m_sounds->playSound("collect", 0.9f, false);
					ItemManager::GetInstance().MarkCollected(part);
					part->SetActive(false);
				}
			}
		}
	}

	ImGui::Separator();

	ImGui::Text("Select Planet Effect:");

	//not working
	if (ImGui::Button("Normal")) {
		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = static_cast<Player*>(obj);
			player->SetEffect(0);
			m_playerSpeed = player->GetPlayerSpeed();
			LogManager::GetInstance().Log("Effect set to Normal");
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Low Gravity")) {
		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = static_cast<Player*>(obj);
			player->SetEffect(1);
			m_playerSpeed = player->GetPlayerSpeed();
			LogManager::GetInstance().Log("Effect set to low");
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Backwards")) {
		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = static_cast<Player*>(obj);
			player->SetEffect(2);
			m_playerSpeed = player->GetPlayerSpeed();
			LogManager::GetInstance().Log("Effect set to reverse");
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("High Gravity")) {
		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = static_cast<Player*>(obj);
			player->SetEffect(3);
			m_playerSpeed = player->GetPlayerSpeed();
			LogManager::GetInstance().Log("Effect set to high");
		}
	}

	ImGui::Separator();
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

			if (m_levelNumber == 1) {
				player->reset();
			}

			player->SetEffect(m_planetEffect);
			m_playerSpeed = player->GetPlayerSpeed();
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
			bool collision = CheckWaterCollision(player);

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
	
	if (inputSystem.GetNumberOfControllersAttached() > 0) {
		XboxMovement(inputSystem, m_currentPlayer, deltaTime);
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_RIGHT) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_RIGHT) == BS_PRESSED
		|| inputSystem.GetKeyState(SDL_SCANCODE_D) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_D) == BS_PRESSED) {
		m_currentPlayer = 1;
		m_currentDirection = 'R';
		SwitchDirection(m_currentDirection);
		updatedPos.x += m_playerSpeed * deltaTime;
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_LEFT) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_LEFT) == BS_PRESSED
		|| inputSystem.GetKeyState(SDL_SCANCODE_A) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_A) == BS_PRESSED) {
		m_currentPlayer = 2;
		m_currentDirection = 'L';
		SwitchDirection(m_currentDirection);
		updatedPos.x -= m_playerSpeed * deltaTime;
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_UP) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_UP) == BS_PRESSED
		|| inputSystem.GetKeyState(SDL_SCANCODE_W) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_W) == BS_PRESSED) {

		if (m_currentPlayer != 2) {
			m_currentPlayer = 1;
		}

		updatedPos.y -= m_playerSpeed * deltaTime;
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_DOWN) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_DOWN) == BS_PRESSED
		|| inputSystem.GetKeyState(SDL_SCANCODE_S) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_S) == BS_PRESSED) {

		if (m_currentPlayer != 1) {
			m_currentPlayer = 2;
		}

		updatedPos.y += m_playerSpeed * deltaTime;
	}

	//detect any collision from the potential movement:

	bool collision = false;

	if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
		Player* player = static_cast<Player*>(obj);
		collision = CheckWaterCollision(player);
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

		m_sounds->playSound("shoot", 0.9f, false);

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

			if (AllEnemiesDefeated() && ItemsCollected()) {
				if (CollectItem(player, m_centerPos)) {
					if (m_levelNumber == 5) {
						m_sounds->playSound("collect", 0.9f, false);
						m_riftVial->SetCollected(true);
						m_gameOver = false;
						NextLevel();
					}
					else {
						m_sounds->playSound("collect", 0.9f, false);
						m_riftVial->SetCollected(true);
						NextLevel();
					}
				}
			}

			for (size_t i = 0; i < m_itemPool->totalCount(); i++) {
				if (GameObject* obj = m_itemPool->getObjectAtIndex(i)) {
					if (obj && dynamic_cast<ShipPart*>(obj)) {
						ShipPart* part = static_cast<ShipPart*>(obj);
						if (part->isActive() && !part->IsCollected() && CollectItem(player, part->Position())) {
							m_sounds->playSound("collect", 0.9f, false);
							ItemManager::GetInstance().MarkCollected(part);
							part->SetActive(false);
						}
					}
				}
			}

			if (GameObject* obj = m_upgradePool->getObjectAtIndex(WeaponState::GetInstance().GetNextWeapon())) {
				if (obj && dynamic_cast<DropableWeapon*>(obj)) {
					DropableWeapon* weapon = static_cast<DropableWeapon*>(obj);
					if (weapon->isActive() && !weapon->IsCollected() && CollectItem(player, weapon->Position())) {
						int nextWeapon = WeaponState::GetInstance().GetNextWeapon();

						SetWeapon(nextWeapon);
						WeaponState::GetInstance().AddWeapon(nextWeapon);
						WeaponState::GetInstance().SetCurrentWeapon(nextWeapon);
						m_sounds->playSound("collect", 0.9f, false);
						weapon->SetActive(false);
					}
				}
			}
		}
	}
}

void Level::XboxMovement(InputSystem& inputSystem, int& m_currentPlayer, float deltaTime)
{
	Vector2 updatedPos = m_playerPosition;
	XboxController* controller = inputSystem.GetController(0);

	if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_START) == BS_HELD || controller->GetButtonState(SDL_CONTROLLER_BUTTON_START) == BS_PRESSED) {
		m_paused = true;
	}

	if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == BS_PRESSED || controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == BS_HELD) {
		m_currentPlayer = 1;
		m_currentDirection = 'R';
		SwitchDirection(m_currentDirection);
		updatedPos.x += m_playerSpeed * deltaTime;
	}

	if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_LEFT) == BS_PRESSED || controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_LEFT) == BS_HELD) {
		m_currentPlayer = 2;
		m_currentDirection = 'L';
		SwitchDirection(m_currentDirection);
		updatedPos.x -= m_playerSpeed * deltaTime;
	}

	if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_UP) == BS_PRESSED || controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_UP) == BS_HELD) {

		if (m_currentPlayer != 2) {
			m_currentPlayer = 1;
		}

		updatedPos.y -= m_playerSpeed * deltaTime;
	}

	if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_DOWN) == BS_PRESSED || controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_DOWN) == BS_HELD) {

		if (m_currentPlayer != 1) {
			m_currentPlayer = 2;
		}

		updatedPos.y += m_playerSpeed * deltaTime;
	}

	//detect any collision from the potential movement:

	bool collision = false;

	if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
		Player* player = static_cast<Player*>(obj);
		collision = CheckWaterCollision(player);
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

	if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_X) == BS_PRESSED) { // weapon swing
		m_currentPlayer = 3;

		m_sounds->playSound("shoot", 0.9f, false);

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

	if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_DOWN) == BS_RELEASED || controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_UP) == BS_RELEASED ||
		controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_LEFT) == BS_RELEASED || controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == BS_RELEASED) {
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

	if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_Y) == BS_HELD || controller->GetButtonState(SDL_CONTROLLER_BUTTON_Y) == BS_PRESSED) {
		m_currentPlayer = 3;

		if (GameObject* obj = m_playerPool->getObjectAtIndex(m_currentPlayer)) {
			Player* player = static_cast<Player*>(obj);
			player->SetRunning();
			player->Position() = m_playerPosition;

			if (AllEnemiesDefeated() && ItemsCollected()) {
				if (CollectItem(player, m_centerPos)) {
					if (m_levelNumber == 5) {
						m_sounds->playSound("collect", 0.9f, false);
						m_riftVial->SetCollected(true);
						m_gameOver = false;
						NextLevel();
					}
					else {
						m_sounds->playSound("collect", 0.9f, false);
						m_riftVial->SetCollected(true);
						NextLevel();
					}
				}
			}

			for (size_t i = 0; i < m_itemPool->totalCount(); i++) {
				if (GameObject* obj = m_itemPool->getObjectAtIndex(i)) {
					if (obj && dynamic_cast<ShipPart*>(obj)) {
						ShipPart* part = static_cast<ShipPart*>(obj);
						if (part->isActive() && !part->IsCollected() && CollectItem(player, part->Position())) {
							m_sounds->playSound("collect", 0.9f, false);
							ItemManager::GetInstance().MarkCollected(part);
							part->SetActive(false);
						}
					}
				}
			}
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

	for (size_t i = 0; i < m_upgradePool->totalCount(); i++) {
		if (GameObject* obj = m_upgradePool->getObjectAtIndex(i)) {
			DropableWeapon* weapon = static_cast<DropableWeapon*>(obj);
			weapon->Initialise(renderer, i);
		}
	}

	if (m_levelNumber == 1) {
		WeaponState::GetInstance().Reset();
		WeaponState::GetInstance().CreateWeaponMap();
		WeaponState::GetInstance().SetCurrentWeapon(0);
	}

	m_currentWeapon = WeaponState::GetInstance().GetCurrentWeapon();
	m_hudParser->SetWeaponHUD(m_currentWeapon);

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
			enemy->SetEnemyType(m_levelDifficulty);
			enemy->SetAttackDamage(m_levelDifficulty, m_gameDifficulty);
		}
	}

	m_boss->Initialise(renderer, m_levelNumber);
	m_boss->SetGameDifficulty(m_gameDifficulty);

	return true;
}

bool Level::ParticlesInitialised(Renderer& renderer)
{
	m_deathParticles->Initialise(renderer, "..\\assets\\skull.png");

	if (!m_deathParticles) return false;

	float colour[3] = { 1.0f, 0.2f, 0.2f };

	m_deathParticles->SetColour(colour);
	m_deathParticles->SetAngles(359.0f, 0.0f);
	m_deathParticles->SetEmitRate(0.2f);
	m_deathParticles->SetBatchSize(4);
	m_deathParticles->SetLifeSpan(0.5f);
	m_deathParticles->SetAcceleration(70.0f);
	m_deathParticles->SetScale(1.0f);

	m_deathParticles->SetActive(false);

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

	Vector2 spawnerPos;

	if (m_spawnEnemies) {
		if (GameObject* obj = m_enemyPool->getObject()) {
			Enemy* enemy = dynamic_cast<Enemy*>(obj);
			if (enemy) {

				int spawner = GetRandom(0, m_spawnerPool->totalCount() - 1);

				if (GameObject* spawnObj = m_spawnerPool->getObjectAtIndex(spawner)) {
					if (EnemySpawner* spawner = dynamic_cast<EnemySpawner*>(spawnObj)) {
						enemy->Position() = spawner->Position();
						spawnerPos = spawner->Position();
					}
				}

				enemy->SetActive(true);
			}
		}

		m_currentEnemies++;

		//10% chance for boss to spawn
		if (!m_boss->isActive() && !m_bossSpawned) {
			float chance = GetRandomPercentage();
			if (chance <= 0.1f) {
				m_boss->Position() = spawnerPos;
				m_boss->SetActive(true);
				m_bossSpawned = true;
			}
		}
	}
}

bool Level::CheckWaterCollision(GameObject* obj)
{
	if (!obj || !obj->isActive()) return false;

	Vector2 pos;
	float width;

	if (Player* player = dynamic_cast<Player*>(obj)) {
		pos = player->Position();
		width = (float)player->GetSpriteWidth();
	}
	else if (Enemy* enemy = dynamic_cast<Enemy*>(obj)) {
		pos = enemy->Position();
		width = (float)enemy->GetSpriteWidth();
	}
	else if (BossEnemy* boss = dynamic_cast<BossEnemy*>(obj)) {
		pos = boss->Position();
		width = (float)boss->GetSpriteWidth();
	}

	Box objectBox(
		pos.x, pos.y, width, width
	);

	auto potentialCollision = m_boundaryCollisionTree->queryRange(objectBox);

	for (auto* object : potentialCollision) {
		if (Water* water = dynamic_cast<Water*>(object)) {
			Box waterBox(water->Position().x, water->Position().y, (float)water->GetSpriteWidth(),
				(float)water->GetSpriteWidth());

			if (CollisionHelper::IsColliding(objectBox, waterBox)) {
				if (Player* player = dynamic_cast<Player*>(obj)) {
					HandlePlayerCollision(waterBox, player);
				}
				else if (Enemy* enemy = dynamic_cast<Enemy*>(obj)) {
					HandleEnemyCollision(waterBox, enemy);
				}
				else if (BossEnemy* boss = dynamic_cast<BossEnemy*>(obj)) {
					HandleEnemyCollision(waterBox, boss);
				}

				return true;
			}
		}
	}

	return false;
}

void Level::HandleEnemyCollision(const Box& collision, GameObject* obj)
{
	Vector2 pos;
	float width;

	if (Enemy* enemy = dynamic_cast<Enemy*>(obj)) {
		pos = enemy->Position();
		width = (float)enemy->GetSpriteWidth();
	}
	else if (BossEnemy* boss = dynamic_cast<BossEnemy*>(obj)) {
		pos = boss->Position();
		width = (float)boss->GetSpriteWidth();
	}

	Box enemyBox(
		pos.x, pos.y,
		width, width);

	Vector2 direction = CollisionHelper::CalcPushDistance(enemyBox, collision);

	float length = CollisionHelper::GetLength(direction);

	if (length != 0) {
		if (Enemy* enemy = dynamic_cast<Enemy*>(obj)) {
			enemy->Position().x += direction.x * 5.0f;
			enemy->Position().y += direction.y * 5.0f;
		}
		else if (BossEnemy* boss = dynamic_cast<BossEnemy*>(obj)) {
			boss->Position().x += direction.x * 5.0f;
			boss->Position().y += direction.y * 5.0f;
		}
	}

	if (Enemy* enemy = dynamic_cast<Enemy*>(obj)) {
		enemy->ResetWander(direction);
	}
	else if (BossEnemy* boss = dynamic_cast<BossEnemy*>(obj)) {
		boss->ResetWander(direction);
	}
}

void Level::HandlePlayerCollision(const Box& collision, Player* player)
{
	Box playerBox(
		player->Position().x, player->Position().y,
		(float)player->GetSpriteWidth(), (float)player->GetSpriteWidth());

	Vector2 direction = CollisionHelper::CalcPushDistance(playerBox, collision);

	float length = CollisionHelper::GetLength(direction);

	if (length != 0) {

		// Push the player back from the water
		player->Position().x += direction.x * 5.0f;
		player->Position().y += direction.y * 5.0f;

		m_playerPrevPosition = m_playerPosition;
		m_playerPosition = player->Position();
	}
}

bool Level::DamageCollision(GameObject* obj, const Box& collision)
{
	bool colliding;

	if (!obj) {
		return false;
	}

	Vector2 pos;
	float width;

	if (Enemy* enemy = dynamic_cast<Enemy*>(obj)) {
		pos = enemy->Position();
		width = (float)enemy->GetSpriteWidth();
	}
	else if (BossEnemy* boss = dynamic_cast<BossEnemy*>(obj)) {
		pos = boss->Position();
		width = (float)boss->GetSpriteWidth();
	}

	colliding = pos.x < collision.x + collision.width &&
		pos.x + width > collision.x &&
		pos.y < collision.y + collision.width &&
		pos.y + width > collision.y;

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
				if (Enemy* enemy = dynamic_cast<Enemy*>(obj)) {
					if (enemy->isActive() && DamageCollision(enemy, playerBox)) {
						Vector2 pushDirection(enemy->Position().x - player->Position().x,
							enemy->Position().y - player->Position().y);

						if (isSwinging) {
							m_sounds->playSound("hit", 0.9f, false);
							enemy->ApplyPushBack(pushDirection);
							enemy->AddDamage(weaponDamage);
							SpawnParticles(enemy->Position());

							if (!enemy->isActive()) {
								SpawnParticles(enemy->Position());

								if (AllEnemiesDefeated()) {
									if (m_itemPool->hasAvailableObjects()) {
										if (GameObject* obj = m_itemPool->getObject()) {
											ShipPart* part = static_cast<ShipPart*>(obj);
											if (part) {
												part->Drop(enemy->Position());
											}
										}
									}
								}
							}
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
				else if (BossEnemy* boss = dynamic_cast<BossEnemy*>(obj)) {
					if (boss->isActive() && DamageCollision(boss, playerBox)) {
						Vector2 pushDirection(boss->Position().x - player->Position().x,
							boss->Position().y - player->Position().y);

						if (isSwinging) {
							m_sounds->playSound("hit", 0.9f, false);
							boss->ApplyPushBack(pushDirection);
							boss->AddDamage(weaponDamage);
							SpawnParticles(boss->Position());

							if (!boss->isActive()) {
								SpawnParticles(boss->Position());
								if (m_levelNumber == 5) {
									if (m_itemPool->hasAvailableObjects()) {
										int offset = -48;
										for (size_t i = 0; i < 3; i++) {
											if (GameObject* obj = m_itemPool->getObject()) {
												ShipPart* part = static_cast<ShipPart*>(obj);
												if (part) {
													Vector2 position = boss->Position();
													position.x += offset;
													part->Drop(position);
													offset += 48;
												}
											}
										}
									}
								}
								else {
									if (GameObject* obj = m_upgradePool->getObjectAtIndex(WeaponState::GetInstance().GetWeaponUpgrade())) {
										DropableWeapon* weapon = static_cast<DropableWeapon*>(obj);
										if (weapon) {
											weapon->Drop(boss->Position());
										}
									}
								}
							}
						}
						else {
							Vector2 pushDirectionPlayer(player->Position().x - boss->Position().x,
								player->Position().y - boss->Position().y);
							player->ApplyPushBack(pushDirectionPlayer);
							player->AddDamage(boss->GetDamageDealt());
							boss->ApplyPushBack(pushDirection);
						}
						break;
					}
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
						float bulletSize = (float)bullet->GetSpriteWidth();
						Box bulletRange(
							bullet->Position().x - bulletSize / 2,
							bullet->Position().y - bulletSize / 2,
							bulletSize,
							bulletSize
						);

						auto potentialCollisions = m_enemyCollisionTree->queryRange(bulletRange);

						for (auto* obj : potentialCollisions) {
							if (Enemy* enemy = dynamic_cast<Enemy*>(obj)) {

								if (enemy->isActive() && DamageCollision(enemy, bulletRange)) {
									m_sounds->playSound("hit", 0.9f, false);
									bullet->SetActive(false);
									Vector2 pushDirection(enemy->Position().x - bullet->Position().x,
										enemy->Position().y - bullet->Position().y);

									enemy->ApplyPushBack(pushDirection);
									enemy->AddDamage(weaponDamage);
									SpawnParticles(enemy->Position());

									if (!enemy->isActive()) {
										SpawnParticles(enemy->Position());
										if (AllEnemiesDefeated()) {
											if (m_itemPool->hasAvailableObjects()) {
												if (GameObject* obj = m_itemPool->getObject()) {
													ShipPart* part = static_cast<ShipPart*>(obj);
													if (part) {
														part->Drop(enemy->Position());
													}
												}
											}
										}
									}

									m_bulletPool->release(bullet);
									break;
								}
							}
							else if (BossEnemy* boss = dynamic_cast<BossEnemy*>(obj)) {
								if (boss->isActive() && DamageCollision(boss, bulletRange)) {
									m_sounds->playSound("hit", 0.9f, false);
									bullet->SetActive(false);
									Vector2 pushDirection(boss->Position().x - bullet->Position().x,
										boss->Position().y - bullet->Position().y);

									boss->ApplyPushBack(pushDirection);
									boss->AddDamage(weaponDamage);
									SpawnParticles(boss->Position());

									if (!boss->isActive()) {
										SpawnParticles(boss->Position());
										if (m_levelNumber == 5) {
											if (m_itemPool->hasAvailableObjects()) {
												int offset = -48;
												for (size_t i = 0; i < 3; i++) {
													if (GameObject* obj = m_itemPool->getObject()) {
														ShipPart* part = static_cast<ShipPart*>(obj);
														if (part) {
															Vector2 position = boss->Position();
															position.x += offset;
															part->Drop(position);
															offset += 48;
														}
													}
												}
											}
										}
										else {
											if (GameObject* obj = m_upgradePool->getObjectAtIndex(WeaponState::GetInstance().GetWeaponUpgrade())) {
												DropableWeapon* weapon = static_cast<DropableWeapon*>(obj);
												if (weapon) {
													weapon->Drop(boss->Position());
												}
											}
										}
									}

									m_bulletPool->release(bullet);
									break;
								}
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
				if (Enemy* enemy = dynamic_cast<Enemy*>(obj)) {
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
				else if (BossEnemy* boss = dynamic_cast<BossEnemy*>(obj)) {
					if (boss->isActive() && DamageCollision(boss, playerBox)) {
						Vector2 pushDirection(boss->Position().x - player->Position().x,
							boss->Position().y - player->Position().y);
						Vector2 pushDirectionPlayer(player->Position().x - boss->Position().x,
							player->Position().y - boss->Position().y);
						player->ApplyPushBack(pushDirectionPlayer);
						player->AddDamage(boss->GetDamageDealt());
						boss->ApplyPushBack(pushDirection);

						break;
					}
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

bool Level::AllEnemiesDefeated()
{
	if (m_enemiesRemoved) return true;

	if (m_maxEnemies != m_currentEnemies && m_levelNumber != 5) return false;

	for (size_t i = 0; i < m_enemyPool->totalCount(); i++) {
		if (GameObject* obj = m_enemyPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<Enemy*>(obj)) {
				Enemy* enemy = static_cast<Enemy*>(obj);
				if (enemy->isActive()) {
					return false;
				}
			}
		}
	}

	return true;
}

bool Level::CollectItem(Player* player, Vector2 position)
{
	float dx = player->Position().x - position.x;
	float dy = player->Position().y - position.y;

	float distSqrd = dx * dx + dy * dy;
	float maxDistance = 30.0f;

	if (distSqrd <= (maxDistance * maxDistance)) {
		return true;
	}

	return false;
}

void Level::SpawnParticles(Vector2 position)
{
	m_deathParticles->SetPosition(position.x, position.y);
	m_particleSpawned = true;
	m_particleTime = 0.0f;
	m_deathParticles->SetActive(true);
}

bool Level::ItemsCollected()
{
	for (size_t i = 0; i < m_itemPool->totalCount(); i++) {
		if (GameObject* obj = m_itemPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<ShipPart*>(obj)) {
				ShipPart* part = static_cast<ShipPart*>(obj);
				if (part->isActive() && !part->IsCollected()) {
					return false;
				}
			}
		}
	}

	return true;
}

void Level::SetRenderColour(string levelType)
{
	if (levelType == "asteria") {
		m_renderColour1 = 91;
		m_renderColour2 = 61;
		m_renderColour3 = 142;
	}
	else if (levelType == "thule") {
		m_renderColour1 = 33;
		m_renderColour2 = 35;
		m_renderColour3 = 78;
	}
	else if (levelType == "sigurd") {
		m_renderColour1 = 71;
		m_renderColour2 = 171;
		m_renderColour3 = 169;
	}
	else {
		m_renderColour1 = 72;
		m_renderColour2 = 108;
		m_renderColour3 = 55;
	}
}

string Level::GetPlanetEffect(int planetEffect)
{
	string effect;
	switch (planetEffect) {
	case 0:
		effect = "Normal Planet";
		break;

	case 1:
		effect = "Low Gravity Planet";
		break;

	case 2:
		effect = "Backwards Planet";
		break;

	case 3:
		effect = "High Gravity Planet";
		break;
	}

	return effect;
}

bool Level::GameOver()
{
	return m_gameOver;
}

void Level::NextLevel()
{
	m_levelDone = true;
}

bool Level::Quit()
{
	return m_pause->Quit();
}

bool Level::Home()
{
	return m_pause->Home();
}

bool Level::GameStatus()
{
	return m_levelDone;
}

