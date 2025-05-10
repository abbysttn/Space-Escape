#ifndef _GAMESTATE_H__
#define _GAMESTATE_H__

#include "Renderer.h"

class Renderer;

enum GameStates {
	SPLASH_AUT,
	SPLASH_FMOD,
	START_MENU,
	DIFFICULTY_MENU,
	INSTRUCTIONS,
	GAMEPLAY,
	PAUSED,
	GAME_OVER
};

class GameState {
public:
	GameState() {}
	virtual ~GameState() = default;
	virtual void Enter() = 0;
	virtual void Update() = 0;
	virtual void Exit() = 0;

	virtual void SetRenderer(Renderer* renderer) { m_renderer = renderer; }
	virtual void SetInputSystem(InputSystem* inputSystem) { m_inputSystem = inputSystem; }
	virtual GameStates GetNextState() const = 0;

protected:

private:

public:

protected:
	Renderer* m_renderer;
	float deltaTime;
	InputSystem* m_inputSystem;

private:
};

#endif // !_GAMESTATE_H__