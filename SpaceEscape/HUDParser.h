#ifndef _HUDPARSER_H__
#define _HUDPARSER_H__

#include "vector2.h"
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

	void SetWeaponHUD(int num);

protected:
	bool FileParsed(Renderer& renderer, const char* filepath);
	bool InitObjects(Renderer& renderer, char tileType, size_t x, size_t y);
	bool WeaponsInitialised(Renderer& renderer);
private:
	HUDParser(const HUDParser& hudParser);
	HUDParser& operator=(const HUDParser& hudParser);

public:

protected:

private:
	GameObjectPool* m_HUDUnderlayTiles;

	GameObjectPool* m_weaponPool;

	DDLevelLoad* m_levelParser;

	Vector2 m_weaponPos;
	float weaponOffset;

	float screenOffsetX;
	float screenOffsetXR;
	float screenOffsetY;
	float screenOffsetYR;
	float screenOffsetCenter;

	float m_tileSize;

	int m_currentWeapon;

};

#endif // !_HUDPARSER_H__
