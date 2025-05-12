#ifndef _FLYINGCUTSCENE_H__
#define _FLYINGCUTSCENE_H__

#include "scene.h"

using namespace std;

class Renderer;
class InputSystem;
class Player;
class Sprite;

class FlyingCutscene : public Scene {
public:
	FlyingCutscene();
	virtual ~FlyingCutscene();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

	bool GetStatus();

protected:

private:
	FlyingCutscene(const FlyingCutscene& startScene);
	FlyingCutscene& operator=(const FlyingCutscene& startScene);

public:

protected:
	Player* m_player;
	Sprite* m_ship;

	bool m_sceneDone;

private:
};

#endif // !_FLYINGCUTSCENE_H__
