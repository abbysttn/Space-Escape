#ifndef _FLYINGCUTSCENE_H__
#define _FLYINGCUTSCENE_H__

#include "scene.h"

using namespace std;

class Renderer;
class InputSystem;
class PlayerShip;
class ParticleEmitter;
class TextRenderer;

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
	FlyingCutscene(const FlyingCutscene& flyingCutScene);
	FlyingCutscene& operator=(const FlyingCutscene& flyingCutScene);

public:

protected:
	PlayerShip* m_ship;
	ParticleEmitter* m_exhaustEmitter;
	TextRenderer* m_gameMessage1;
	TextRenderer* m_gameMessage2;

	float m_particleOffset;

	bool m_sceneDone;

	float m_textTime;
	float m_textTimer;

private:
};

#endif // !_FLYINGCUTSCENE_H__
