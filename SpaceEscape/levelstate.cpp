#include "levelstate.h"

#include "level.h"
#include "inlinehelpers.h"

#include "logmanager.h"
#include "imgui/imgui.h"

LevelState::LevelState() : m_scene(nullptr), m_nextState(GameStates::NONE) {}

void LevelState::Enter()
{
	if (!m_renderer) return;

	if (m_selectedLevelMap != -1) {
		m_levelMap = m_selectedLevelMap;
	}

	if (m_selectedPlanet != "") {
		m_levelType = m_selectedPlanet;
	}

	int levelMap = (m_selectedLevelMap != -1) ? m_selectedLevelMap : GetRandomLevelMap();
	string planetType = (m_selectedPlanet != "") ? m_selectedPlanet : GetRandomLevelType();

	m_scene = new Level(planetType, GetRandomLevelDifficulty(), levelMap, GetDifficulty(), m_levelNumber, GetRandomPlanetEffect());

	if (m_scene) m_scene->Initialise(*m_renderer);

	m_selectedLevelMap = -1;
	m_selectedPlanet = "";
}

void LevelState::Update(float deltatime)
{
	if (m_scene && m_inputSystem) {
		m_scene->Process(deltatime, *m_inputSystem);

		if (m_scene->Home()) {
			m_nextState = GameStates::START_MENU;
		}

		if (m_scene->Quit()) {
			m_gameLooping = false;
			m_nextState = GameStates::NONE;
		}

		if (m_levelNumber == 5 && m_scene->GameStatus()) {
			if (!m_scene->GameOver()) {
				SetGameWon(true);
			}
			m_nextState = GameStates::GAME_OVER;
			m_levelNumber = 1;
		} else if (m_scene->GameStatus()) {
			if (m_scene->GameOver()) {
				SetGameWon(false);
				m_nextState = GameStates::GAME_OVER;
			}
			m_levelNumber++;
			m_nextState = GameStates::TRANSITION;
		}
	}
}

void LevelState::Draw()
{
	if (m_scene && m_renderer) m_scene->Draw(*m_renderer);
}

void LevelState::DebugDraw()
{
	ImGui::Text("Level Details:");
	ImGui::Text("Game Difficulty - %c", m_gameDifficulty);
	ImGui::Text("Level Difficulty - %c", m_levelDifficulty);
	ImGui::Text("Level Map - %d", m_levelMap);
	ImGui::Text("Level Number - %d", m_levelNumber);
	ImGui::Text("Level Type - %s", m_levelType.c_str());

	if (m_scene) m_scene->DebugDraw();

	ImGui::Separator();

	ImGui::Text("Level Select:");
	if (ImGui::Button("Level 1")) {
		m_nextState = GameStates::TRANSITION;
		m_levelNumber = 1;
	}
	ImGui::SameLine();
	if (ImGui::Button("Level 2")) {
		m_nextState = GameStates::TRANSITION;
		m_levelNumber = 2;
	}
	ImGui::SameLine();
	if (ImGui::Button("Level 3")) {
		m_nextState = GameStates::TRANSITION;
		m_levelNumber = 3;
	}
	ImGui::SameLine();
	if (ImGui::Button("Level 4")) {
		m_nextState = GameStates::TRANSITION;
		m_levelNumber = 4;
	}
	ImGui::SameLine();
	if (ImGui::Button("Level 5")) {
		m_nextState = GameStates::TRANSITION;
		m_levelNumber = 5;
	}

	ImGui::Separator();

	ImGui::Text("Next Level Map:");
	ImGui::RadioButton("Map 1", &m_selectedLevelMap, 1); ImGui::SameLine();
	ImGui::RadioButton("Map 2", &m_selectedLevelMap, 2); ImGui::SameLine();
	ImGui::RadioButton("Map 3", &m_selectedLevelMap, 3); ImGui::SameLine();
	ImGui::RadioButton("Map 4", &m_selectedLevelMap, 4);
	ImGui::RadioButton("Map 5", &m_selectedLevelMap, 5); ImGui::SameLine();
	ImGui::RadioButton("Map 6", &m_selectedLevelMap, 6); ImGui::SameLine();
	ImGui::RadioButton("Map 7", &m_selectedLevelMap, 7); ImGui::SameLine();
	ImGui::RadioButton("Map 8", &m_selectedLevelMap, 8);

	ImGui::Separator();

	ImGui::Text("Next Planet Select:");
	if (ImGui::Button("Asteria")) {
		m_selectedPlanet = "asteria";
	}
	ImGui::SameLine();
	if (ImGui::Button("Thule")) {
		m_selectedPlanet = "thule";
	}
	ImGui::SameLine();
	if (ImGui::Button("Sigurd")) {
		m_selectedPlanet = "sigurd";
	}

	ImGui::Text("Selected Planet: %s", m_selectedPlanet.c_str());

	ImGui::Separator();
	
}

void LevelState::Exit()
{
	delete m_scene;
	m_scene = 0;
	m_nextState = GameStates::NONE;
}

GameStates LevelState::GetNextState() const
{
	return m_nextState;
}

char LevelState::GetRandomLevelDifficulty()
{
	int randomDifficulty;

	char difficulty;

	switch (m_levelNumber) {
	case 1:
		difficulty = 'E';
		break;

	case 2:
		randomDifficulty = GetRandom(1, 2);
		if (randomDifficulty == 1) {
			difficulty = 'E';
		}
		else {
			difficulty = 'M';
		}
		break;

	case 3:
		randomDifficulty = GetRandom(1, 3);
		if (randomDifficulty == 1) {
			difficulty = 'E';
		}
		else if (randomDifficulty == 2) {
			difficulty = 'M';
		}
		else {
			difficulty = 'H';
		}
		break;

	case 4:
		randomDifficulty = GetRandom(1, 2);
		if (randomDifficulty == 1) {
			difficulty = 'M';
		}
		else {
			difficulty = 'H';
		}
		break;

	case 5:
		difficulty = 'H';
		break;

	default:
		difficulty = 'M';
		break;
	}

	m_levelDifficulty = difficulty;
	return difficulty;
}

char LevelState::GetDifficulty()
{
	char gameDifficulty;

	switch (GetGameDifficulty()) {
	case GameDifficulty::EASY:
		gameDifficulty = 'E';
		break;
	case GameDifficulty::NORMAL:
		gameDifficulty = 'N';
		break;
	case GameDifficulty::HARD:
		gameDifficulty = 'H';
		break;
	default:
		gameDifficulty = 'N';
		break;
	}

	m_gameDifficulty = gameDifficulty;

	return gameDifficulty;
}

int LevelState::GetRandomLevelMap()
{
	int levelNum = GetRandom(1, 8);

	m_levelMap = levelNum;

	return levelNum;
}

string LevelState::GetRandomLevelType()
{
	int random = GetRandom(1, 3);

	string levelType;

	switch (random) {
	case 1:
		levelType = "sigurd";
		break;

	case 2:
		levelType = "asteria";
		break;

	case 3:
		levelType = "thule";
		break;
	}

	m_levelType = levelType;

	return levelType;
}

int LevelState::GetRandomPlanetEffect()
{
	int randomEffect = GetRandom(0, 3);

	if (m_levelNumber == 5 || m_levelNumber == 1) {
		randomEffect = 0;
	}

	switch (randomEffect) {
	case 0:
		m_planetEffect = "Normal Planet";
		break;

	case 1:
		m_planetEffect = "Low Gravity Planet";
		break;

	case 2:
		m_planetEffect = "Backwards Planet";
		break;

	case 3:
		m_planetEffect = "High Gravity Planet";
		break;
	}

	return randomEffect;
}
