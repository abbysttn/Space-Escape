#include "gamestatemanager.h"

#include "Renderer.h"
#include "gamestate.h"
#include "inputsystem.h"

#include "splashautstate.h"
#include "splashfmodstate.h"
#include "startmenustate.h"
#include "difficultymenustate.h"
#include "levelstate.h"
#include "leveltransitionstate.h"

GameStateManager::GameStateManager(Renderer& renderer, InputSystem& inputSystem) : m_renderer(renderer), m_inputSystem(inputSystem)
, m_currentState(nullptr), m_gameLooping(true)
{
	m_states[GameStates::SPLASH_FMOD] = std::make_unique<SplashFMODState>();
	m_states[GameStates::DIFFICULTY_MENU] = std::make_unique<DifficultyMenuState>();
	m_states[GameStates::SPLASH_AUT] = std::make_unique<SplashAUTState>();
	m_states[GameStates::START_MENU] = std::make_unique<StartMenuState>();
	m_states[GameStates::GAMEPLAY] = std::make_unique<LevelState>();
	m_states[GameStates::TRANSITION] = std::make_unique<LevelTransitionState>();

	ChangeState(GameStates::GAMEPLAY);
}

void GameStateManager::ChangeState(GameStates newState)
{
	if (m_currentState) {
		if (dynamic_cast<DifficultyMenuState*>(m_currentState)) {
			m_currentDifficulty = m_currentState->GetGameDifficulty();
		}
		m_currentState->Exit();
	}

	auto state = m_states.find(newState);

	if (state != m_states.end()) {
		m_currentState = state->second.get();
		m_currentState->SetRenderer(&m_renderer);
		m_currentState->SetInputSystem(&m_inputSystem);

		if (newState == GameStates::GAMEPLAY) {
			m_currentState->SetDifficulty(m_currentDifficulty);
		}

		m_currentState->Enter();
	}
}

void GameStateManager::Update(float deltatime)
{
	if (m_currentState) {
		m_currentState->Update(deltatime);
		m_gameLooping = m_currentState->GetGameStatus();

		GameStates nextState = m_currentState->GetNextState();
		if (nextState != GameStates::NONE) {
			ChangeState(nextState);
		}
	}
}

void GameStateManager::Draw()
{
	if (m_currentState) {
		m_currentState->Draw();
	}
}

void GameStateManager::DebugDraw()
{
	if (m_currentState) {
		m_currentState->DebugDraw();
	}
}

bool GameStateManager::GetGameStatus()
{
	return m_gameLooping;
}
