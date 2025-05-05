#ifndef _HUDPARSER_H__
#define _HUDPARSER_H__

#include "scene.h"

class Scene;
class Renderer;
class DDLevelLoad;
class GameObjectPool;

class HUDParser : public Scene {
public:
	HUDParser();
	virtual ~HUDParser();
	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

protected:
	bool FileParsed(Renderer& renderer, const char* filepath);
	bool InitObjects(Renderer& renderer, char tileType, size_t x, size_t y);
private:
	HUDParser(const HUDParser& hudParser);
	HUDParser& operator=(const HUDParser& hudParser);

public:

protected:

private:
	GameObjectPool* m_HUDUnderlayTiles;

	DDLevelLoad* m_levelParser;

	float screenOffsetX;
	float screenOffsetXR;
	float screenOffsetY;
	float screenOffsetYR;
	float screenOffsetCenter;

	float m_tileSize;

};

#endif // !_HUDPARSER_H__
