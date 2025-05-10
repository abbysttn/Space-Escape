#include "gamestatemanager.h"

#include "Renderer.h"
#include "gamestate.h"
#include "inputsystem.h"

#include "splashautstate.h"

GameStateManager::GameStateManager(Renderer& renderer, InputSystem& inputSystem) : m_renderer(renderer), m_inputSystem(inputSystem)
{
	m_states[GameStates::SPLASH_AUT] = std::make_unique<SplashAUTState>();

	ChangeState(GameStates::SPLASH_AUT);
}

void GameStateManager::ChangeState(GameStates newState)
{
	if (m_currentState) {
		m_currentState->Exit();
	}

	m_currentState = m_states[newState].get();
	m_currentState->SetRenderer(&m_renderer);
	m_currentState->Enter();
}

void GameStateManager::Update()
{
	if (m_currentState) {
		m_currentState->Update();

		GameStates nextState = m_currentState->GetNextState();
		if (nextState) {
			ChangeState(nextState);
		}
	}
}
