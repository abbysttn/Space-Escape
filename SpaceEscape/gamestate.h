#ifndef _GAMESTATE_H__
#define _GAMESTATE_H__

#include "Renderer.h"
#include "inputsystem.h"

class Renderer;
class InputSystem;

enum GameStates {
	NONE = 0,
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
	GameState() : m_renderer(nullptr), m_inputSystem(nullptr), m_gameLooping(true) {}
	virtual ~GameState() = default;
	virtual void Enter() = 0;
	virtual void Update(float deltatime) = 0;
	virtual void Draw() = 0;
	virtual void DebugDraw() = 0;
	virtual void Exit() = 0;

	virtual void SetRenderer(Renderer* renderer) { m_renderer = renderer; }
	virtual void SetInputSystem(InputSystem* inputSystem) { m_inputSystem = inputSystem; }
	virtual GameStates GetNextState() const = 0;

	bool GetGameStatus() { return m_gameLooping; }

protected:

private:
	GameState(const GameState& gameState);
	GameState& operator=(const GameState& gameState);

public:

protected:
	Renderer* m_renderer;
	InputSystem* m_inputSystem;

	bool m_gameLooping;

private:
};

#endif // !_GAMESTATE_H__