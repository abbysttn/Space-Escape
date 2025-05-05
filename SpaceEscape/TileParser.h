#ifndef _TILEPARSER_H__
#define _TILEPARSER_H__

#include "scene.h"
#include <string>
#include <memory>
#include "vector2.h"

using namespace std;

class Scene;
class Renderer;
class DDLevelLoad;
class GameObjectPool;
class QuadTree;
struct Box;

class TileParser : public Scene {
public:
	TileParser(string lType, int lNum);
	virtual ~TileParser();
	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

	Vector2 GetPlayerStartPosition();
	GameObjectPool* GetWaterPool();


protected:
	bool FileParsed(Renderer& renderer);
	bool InitObjects(Renderer& renderer, char tileType, size_t x, size_t y, int layer);
private:
	TileParser(const TileParser& tileParser);
	TileParser& operator=(const TileParser& tileParser);

public:

protected:

private:
	GameObjectPool* m_cornerPool;
	GameObjectPool* m_edgeCornerPool;
	GameObjectPool* m_edgePool;
	GameObjectPool* m_centerPool;
	GameObjectPool* m_waterPool;
	GameObjectPool* m_propPool;

	GameObjectPool* m_bridgePool;

	string levelType;
	int levelNum;
	float m_tileSize;

	bool playerPositioned;
	Vector2 m_playerStartPos;

	DDLevelLoad* m_levelParser;

	float screenOffsetX;
	float screenOffsetY;

};

#endif // !_TILEPARSER_H__
