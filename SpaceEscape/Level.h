#ifndef _LEVEL_H__
#define _LEVEL_H__

#include "scene.h"
#include "vector2.h"
#include <string>
#include <memory>

using namespace std;

class Renderer;
class GameObjectPool;
class GameObject;
class DDLevelLoad;
class LogManager;
class InputSystem;
class QuadTree;
struct Box;
class SoundSystem;

class HUDParser;
class TileParser;
class CollisionHelper;
class ParticleEmitter;

class Player;
class Water;
class Enemy;
class BossEnemy;

class RiftVial;

class Level : public Scene {
public:
	Level(string levelType, char levelDifficulty, int levelMap, char gameDifficulty, int levelNumber);
	virtual ~Level();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

	bool GameOver();
	void NextLevel();

	bool GameStatus();

protected:
	bool PlayerInitialised(Renderer& renderer);
	void PlayerMovement(InputSystem& inputSystem, int& m_currentPlayer, float deltaTime);
	bool IsColliding(const Box& playerBox, Water* water);
	bool WeaponsInitialised(Renderer& renderer);
	bool BulletsInitialised(Renderer& renderer);
	bool EnemiesInitialised(Renderer& renderer);
	bool ParticlesInitialised(Renderer& renderer);

	void AddWaterCollision();

	void SetWeapon(int num);

	void SwitchDirection(char direction);

	void SpawnEnemy();

	bool CheckWaterCollision(GameObject* obj);

	void HandleEnemyCollision(const Box& collision, GameObject* obj);
	void HandlePlayerCollision(const Box& collision, Player* player);

	bool DamageCollision(GameObject* obj, const Box& collision);
	void DoDamage();

	bool AllEnemiesDefeated();

	bool CollectItem(Player* player, Vector2 position);
	void SpawnParticles(Vector2 position);

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
	GameObjectPool* m_itemPool;

	BossEnemy* m_boss;
	ParticleEmitter* m_deathParticles;

	unique_ptr<QuadTree> m_boundaryCollisionTree;
	unique_ptr<QuadTree> m_enemyCollisionTree;

	HUDParser* m_hudParser;
	TileParser* m_tileParser;

	int m_currentPlayer;
	int m_currentWeapon;
	char m_currentDirection = 'R';

	float m_tileSize;
	float m_playerSize;
	float screenOffsetX;
	float screenOffsetY;

	bool playerPositioned;

	Vector2 m_centerPos;
	Vector2 m_playerPosition;
	Vector2 m_playerPrevPosition;

	bool m_playerPushed;

	SoundSystem* m_sounds;

	float m_cooldownTime;
	float m_currentCooldown;

	float m_enemySpawnTimer;
	float m_enemySpawnTime;
	int m_maxEnemies;
	int m_currentEnemies;

	bool m_playerAlive;
	bool m_invulnerability;

	string m_levelType;
	char m_levelDifficulty;
	int m_levelMap;
	int m_levelNumber;
	char m_gameDifficulty;

	bool m_levelDone;
	bool m_gameOver;


	RiftVial* m_riftVial;
	float m_particleTime;
	float m_particleMaxTime;
	bool m_particleSpawned;

private:
};

#endif // !_LEVEL_H__
