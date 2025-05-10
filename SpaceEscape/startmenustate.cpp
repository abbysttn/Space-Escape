#include "startmenustate.h"

#include "startscene.h"

StartMenuState::StartMenuState() : m_scene(nullptr), m_nextState(GameStates::NONE) {}

void StartMenuState::Enter()
{
	if (!m_renderer) return;

	m_scene = new StartScene();
	if (m_scene) m_scene->Initialise(*m_renderer);
}

void StartMenuState::Update(float deltatime)
{
	if (m_scene && m_inputSystem) {
		m_scene->Process(deltatime, *m_inputSystem);
		if (m_scene->GetStatus()) {
			char selection = m_scene->GetSelection();

			switch (selection) {
			case 'S':
				m_nextState = GameStates::DIFFICULTY_MENU;
				break;

			case 'I':
				break;

			case 'E':
				m_gameLooping = false;
				m_nextState = GameStates::NONE;
				break;
			}
		}
	}
}

void StartMenuState::Draw()
{
	if (m_scene && m_renderer) m_scene->Draw(*m_renderer);
}

void StartMenuState::DebugDraw()
{
	if (m_scene) m_scene->DebugDraw();
}

void StartMenuState::Exit()
{
	delete m_scene;
	m_scene = 0;
}

GameStates StartMenuState::GetNextState() const
{
	return m_nextState;
}
