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

private:
	Level(const Level& level);
	Level& operator=(const Level& level);

public:

protected:
	GameObjectPool* m_cornerPool;
	GameObjectPool* m_straightPool;
	GameObjectPool* m_edgePool;
	GameObjectPool* m_centerPool;

	float m_tileSize;

	DDLevelLoad* m_levelParser;


private:
};

#endif // !_LEVEL_H__
