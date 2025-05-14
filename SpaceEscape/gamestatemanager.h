#ifndef _GAMESTATEMANAGER_H__
#define _GAMESTATEMANAGER_H__

#define USE_SOUND 1

#include <unordered_map>
#include <memory>

class Renderer;
class InputSystem;
enum GameStates;
enum GameDifficulty;
class GameState;
class SoundSystem;

class GameStateManager {
public:
	GameStateManager(Renderer& renderer, InputSystem& inputSystem);

	void ChangeState(GameStates newState);
	void Update(float deltatime);
	void Draw();
	void DebugDraw();

	bool GetGameStatus();

protected:

private:
	GameStateManager(const GameStateManager& gameStateManager);
	GameStateManager& operator=(const GameStateManager& gameStateManager);

public:

protected:
	std::unordered_map<GameStates, std::unique_ptr<GameState>> m_states;
	GameState* m_currentState;
	GameDifficulty m_currentDifficulty;
	Renderer& m_renderer;
	InputSystem& m_inputSystem;
	SoundSystem* m_soundSystem;

	bool m_wonGame;

	bool m_gameLooping;

private:
};

#endif // !_GAMESTATEMANAGER_H__
