#include "gamestatemanager.h"

#include "Renderer.h"
#include "gamestate.h"
#include "inputsystem.h"
#include "soundsystem.h"

#include "splashautstate.h"
#include "splashfmodstate.h"
#include "startmenustate.h"
#include "difficultymenustate.h"
#include "levelstate.h"
#include "leveltransitionstate.h"
#include "flyingcutscenestate.h"
#include "gameoverstate.h"

#include "itemmanager.h"

GameStateManager::GameStateManager(Renderer& renderer, InputSystem& inputSystem) : m_renderer(renderer), m_inputSystem(inputSystem)
, m_currentState(nullptr), m_gameLooping(true)
{
	m_states[GameStates::SPLASH_FMOD] = std::make_unique<SplashFMODState>();
	m_states[GameStates::DIFFICULTY_MENU] = std::make_unique<DifficultyMenuState>();
	m_states[GameStates::SPLASH_AUT] = std::make_unique<SplashAUTState>();
	m_states[GameStates::START_MENU] = std::make_unique<StartMenuState>();
	m_states[GameStates::GAMEPLAY] = std::make_unique<LevelState>();
	m_states[GameStates::TRANSITION] = std::make_unique<LevelTransitionState>();
	m_states[GameStates::FLYING_CUTSCENE] = std::make_unique<FlyingCutsceneState>();
	m_states[GameStates::GAME_OVER] = std::make_unique<GameOverState>();

	ChangeState(GameStates::DIFFICULTY_MENU);

#if USE_SOUND
	m_soundSystem = new SoundSystem();
	m_soundSystem->initialise();
	m_soundSystem->loadSound("background", "..\\assets\\background.wav", true);
#endif
}

void GameStateManager::ChangeState(GameStates newState)
{
	if (m_currentState) {
		if (dynamic_cast<DifficultyMenuState*>(m_currentState)) {
			m_currentDifficulty = m_currentState->GetGameDifficulty();
			ItemManager::GetInstance().CreatePool(m_renderer);

			//reset on game over
		}
		if (dynamic_cast<LevelState*>(m_currentState)) {
			m_wonGame = m_currentState->GetGameWon();
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
		if (newState == GameStates::GAME_OVER) {
			m_currentState->SetGameWon(m_wonGame);
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

	m_soundSystem->playSound("background", 0.1f, true);
	m_soundSystem->update();
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
