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
#include "instructionstate.h"

#include "itemmanager.h"

#include "imgui/imgui.h"

#include "logmanager.h"

GameStateManager::GameStateManager(Renderer& renderer, InputSystem& inputSystem) : m_renderer(renderer), m_inputSystem(inputSystem)
, m_currentState(nullptr), m_gameLooping(true), m_debugging(false)
{
	m_states[GameStates::SPLASH_FMOD] = std::make_unique<SplashFMODState>();
	m_states[GameStates::DIFFICULTY_MENU] = std::make_unique<DifficultyMenuState>();
	m_states[GameStates::SPLASH_AUT] = std::make_unique<SplashAUTState>();
	m_states[GameStates::START_MENU] = std::make_unique<StartMenuState>();
	m_states[GameStates::GAMEPLAY] = std::make_unique<LevelState>();
	m_states[GameStates::TRANSITION] = std::make_unique<LevelTransitionState>();
	m_states[GameStates::FLYING_CUTSCENE] = std::make_unique<FlyingCutsceneState>();
	m_states[GameStates::GAME_OVER] = std::make_unique<GameOverState>();
	m_states[GameStates::INSTRUCTIONS] = std::make_unique<InstructionState>();

	ChangeState(GameStates::SPLASH_AUT);

	m_soundSystem = new SoundSystem();
	m_soundSystem->initialise();
	m_soundSystem->loadSound("background", "..\\assets\\background.wav", true);
}

void GameStateManager::ChangeState(GameStates newState)
{
	if (m_currentState) {
		if (dynamic_cast<DifficultyMenuState*>(m_currentState)) {
			m_currentDifficulty = m_currentState->GetGameDifficulty();
			ItemManager::GetInstance().CreatePool(m_renderer);

			//need to keep track of weapon upgrades within the game, but reset on game over
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

		m_renderer.ClearTextures();
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

	if (m_inputSystem.GetKeyState(SDL_SCANCODE_INSERT) == BS_PRESSED) {
		m_debugging = !m_debugging;
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
	ImGui::Text("Current Scene: %s", GetCurrentScene().c_str());

	ImGui::Text("Current Frame Rate: %.2f", ImGui::GetIO().Framerate);

	ImGui::Separator();

	LogManager::GetInstance().DebugDraw();

	ImGui::Separator();

	if (m_currentState) {
		m_currentState->DebugDraw();
	}
}

bool GameStateManager::GetGameStatus()
{
	return m_gameLooping;
}

bool GameStateManager::GetDebuggingStatus()
{
	return m_debugging;
}

string GameStateManager::GetCurrentScene()
{
	if (!m_currentState) return "No State";

	if (dynamic_cast<SplashAUTState*>(m_currentState)) {
		return "Splash (AUT)";
	}
	else if (dynamic_cast<SplashFMODState*>(m_currentState)) {
		return "Splash (FMOD)";
	}
	else if (dynamic_cast<StartMenuState*>(m_currentState)) {
		return "Start Menu";
	}
	else if (dynamic_cast<DifficultyMenuState*>(m_currentState)) {
		return "Difficulty Menu";
	}
	else if (dynamic_cast<LevelState*>(m_currentState)) {
		return "Gameplay Level";
	}
	else if (dynamic_cast<LevelTransitionState*>(m_currentState)) {
		return "Level Transition";
	}
	else if (dynamic_cast<FlyingCutsceneState*>(m_currentState)) {
		return "Flying Cutscene";
	}
	else if (dynamic_cast<GameOverState*>(m_currentState)) {
		return "Game Over";
	}
	else if (dynamic_cast<InstructionState*>(m_currentState)) {
		return "Instructions";
	}

	return "Unknown State";
}
