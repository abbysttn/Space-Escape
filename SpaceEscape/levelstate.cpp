#include "levelstate.h"

#include "level.h"
#include "inlinehelpers.h"

LevelState::LevelState() : m_scene(nullptr), m_nextState(GameStates::NONE) {}

void LevelState::Enter()
{
	if (!m_renderer) return;

	m_scene = new Level("summer", GetRandomLevelDifficulty(), GetRandomLevelMap(), GetDifficulty(), m_levelNumber);
	if (m_scene) m_scene->Initialise(*m_renderer);
}

void LevelState::Update(float deltatime)
{
	if (m_scene && m_inputSystem) {
		m_scene->Process(deltatime, *m_inputSystem);
		if (m_scene->GameStatus()) {
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
	if (m_scene) m_scene->DebugDraw();

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

	switch (m_levelNumber) {
	case 1:
		return 'E';

	case 2:
		randomDifficulty = GetRandom(1, 2);
		if (randomDifficulty == 1) {
			return 'E';
		}
		else {
			return 'M';
		}
		break;

	case 3:
		randomDifficulty = GetRandom(1, 3);
		if (randomDifficulty == 1) {
			return 'E';
		}
		else if (randomDifficulty == 2) {
			return 'M';
		}
		else {
			return 'H';
		}
		break;

	case 4:
		randomDifficulty = GetRandom(1, 2);
		if (randomDifficulty == 1) {
			return 'M';
		}
		else {
			return 'H';
		}
		break;

	case 5:
		return 'H';

	default:
		return 'M';
	}
}

char LevelState::GetDifficulty()
{
	switch (GetGameDifficulty()) {
	case GameDifficulty::EASY:
		return 'E';
	case GameDifficulty::NORMAL:
		return 'N';
	case GameDifficulty::HARD:
		return 'H';
	default:
		return 'N';
	}
}

int LevelState::GetRandomLevelMap()
{
	int levelNum = GetRandom(1, 8);

	return levelNum;
}
