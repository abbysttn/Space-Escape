#ifndef _GAMESTATEMANAGER_H__
#define _GAMESTATEMANAGER_H__

#include <map>
#include <memory>

class Renderer;
class InputSystem;
enum GameStates;
class GameState;

class GameStateManager {
public:
	GameStateManager(Renderer& renderer, InputSystem& inputSystem);

	void ChangeState(GameStates newState);
	void Update();

protected:

private:

public:

protected:
	std::map<GameStates, std::unique_ptr<GameState>> m_states;
	GameState* m_currentState;
	Renderer& m_renderer;
	InputSystem& m_inputSystem;

private:
};

#endif // !_GAMESTATEMANAGER_H__
