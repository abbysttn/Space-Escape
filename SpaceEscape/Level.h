#ifndef _LEVEL_H__
#define _LEVEL_H__

#include "scene.h"
#include "vector2.h"
#include <string>
#include <memory>

using namespace std;

class Renderer;
class GameObjectPool;
class DDLevelLoad;
class LogManager;
class InputSystem;
class QuadTree;
struct Box;
class SoundSystem;

class HUDParser;
class TileParser;

class Player;
class Water;
class Enemy;

class Level : public Scene {
public:
	Level();
	virtual ~Level();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

protected:
	bool PlayerInitialised(Renderer& renderer);
	void PlayerMovement(InputSystem& inputSystem, int& m_currentPlayer, float deltaTime);
	bool IsColliding(const Box& playerBox, Water* water);
	bool WeaponsInitialised(Renderer& renderer);
	bool BulletsInitialised(Renderer& renderer);
	bool EnemiesInitialised(Renderer& renderer);

	void AddWaterCollision();

	void SetWeapon(int num);

	void SwitchDirection(char direction);

	void SpawnEnemy();

	bool EnemyColliding(Enemy* enemy);
	void HandleEnemyCollision(const Box& collision, Enemy* enemy);
	bool PlayerColliding(Player* player);
	void HandlePlayerCollision(const Box& collision, Player* player);

	bool DamageCollision(Enemy* enemy, const Box& collision);
	void DoDamage();

	bool IsPositionValid(Vector2& position);

private:
	Level(const Level& level);
	Level& operator=(const Level& level);

public:

protected:
	GameObjectPool* m_playerPool;
	GameObjectPool* m_waterPool;
	GameObjectPool* m_bulletPool;
	GameObjectPool* m_weaponPool;
	GameObjectPool* m_spawnerPool;
	GameObjectPool* m_enemyPool;

	unique_ptr<QuadTree> m_boundaryCollisionTree;
	unique_ptr<QuadTree> m_enemyCollisionTree;

	HUDParser* m_hudParser;
	TileParser* m_tileParser;

	int m_currentPlayer;
	int m_levelNumber;
	int m_layerNumber;
	int m_currentWeapon;
	char m_currentDirection = 'R';

	float m_tileSize;
	float m_playerSize;
	float screenOffsetX;
	float screenOffsetY;

	bool playerPositioned;

	Vector2 m_playerPosition;
	Vector2 m_playerPrevPosition;

	bool m_playerPushed;

	SoundSystem* m_soundSystem;

	string levelType;
	string levelDifficulty;

	float m_cooldownTime;
	float m_currentCooldown;

	float m_enemySpawnTimer;
	float m_enemySpawnTime;
	int m_maxEnemies;
	int m_currentEnemies;

private:
};

#endif // !_LEVEL_H__
