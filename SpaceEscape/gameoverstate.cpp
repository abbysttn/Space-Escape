#include "gameoverstate.h"

#include "gameoverscene.h"

GameOverState::GameOverState() : m_scene(nullptr), m_nextState(GameStates::NONE) {}

void GameOverState::Enter()
{
	if (!m_renderer) return;

	m_scene = new GameOverScene(m_wonGame);
	if (m_scene) m_scene->Initialise(*m_renderer);
}

void GameOverState::Update(float deltatime)
{
	if (m_scene && m_inputSystem) {
		m_scene->Process(deltatime, *m_inputSystem);
		if (m_scene->GetStatus()) {
			m_nextState = GameStates::START_MENU;
		}
	}
}

void GameOverState::Draw()
{
	if (m_scene && m_renderer) m_scene->Draw(*m_renderer);
}

void GameOverState::DebugDraw()
{
	if (m_scene) m_scene->DebugDraw();
}

void GameOverState::Exit()
{
	delete m_scene;
	m_scene = 0;
}

GameStates GameOverState::GetNextState() const
{
	return m_nextState;
}
