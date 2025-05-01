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
class SoundSystem;

class Player;
class Water;

class Level : public Scene {
public:
	Level();
	virtual ~Level();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

protected:
	bool InitObjects(Renderer& renderer, char tileType, size_t x, size_t y);
	void PlayerMovement(InputSystem& inputSystem, int& m_currentPlayer, float deltaTime);
	bool IsColliding(Player* player, Water* water);
	bool IsMovingAway(Water* water);

private:
	Level(const Level& level);
	Level& operator=(const Level& level);

public:

protected:
	GameObjectPool* m_cornerPool;
	GameObjectPool* m_edgeCornerPool;
	GameObjectPool* m_edgePool;
	GameObjectPool* m_centerPool;
	GameObjectPool* m_waterPool;

	GameObjectPool* m_playerPool;

	unique_ptr<QuadTree> m_collisionTree;

	int m_currentPlayer;

	float m_tileSize;
	float screenOffsetX;
	float screenOffsetY;

	Vector2 m_playerPosition;
	Vector2 m_playerPrevPosition;

	SoundSystem* m_soundSystem;

	string levelType;

	DDLevelLoad* m_levelParser;


private:
};

#endif // !_LEVEL_H__
