#ifndef _LEVEL_H__
#define _LEVEL_H__

#include "scene.h"
#include <string>

using namespace std;

class Renderer;
class GameObjectPool;
class DDLevelLoad;
class LogManager;

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

	float m_tileSize;
	float screenOffsetX;
	float screenOffsetY;

	string levelType;

	DDLevelLoad* m_levelParser;


private:
};

#endif // !_LEVEL_H__
