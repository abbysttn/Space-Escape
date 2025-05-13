#ifndef _GAMEOVERSCENE_H__
#define _GAMEOVERSCENE_H__

#include "scene.h"

using namespace std;

class Renderer;
class InputSystem;
class PlayerShip;
class TextRenderer;
class ParticleEmitter;

class GameOverScene : public Scene {
public:
	GameOverScene(bool won);
	virtual ~GameOverScene();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

	bool GetStatus();

protected:

private:
	GameOverScene(const GameOverScene& gameOverScene);
	GameOverScene& operator=(const GameOverScene& gameOverScene);

public:

protected:
	PlayerShip* m_ship;
	TextRenderer* m_gameMessage;
	ParticleEmitter* m_exhaustEmitter;

	bool m_sceneDone;
	bool m_gameWon;

	float m_particleOffset;

private:
};

#endif // !_GAMEOVERSCENE_H__
