#include "difficultymenustate.h"

#include "difficultyscene.h"

DifficultyMenuState::DifficultyMenuState() : m_scene(nullptr), m_nextState(GameStates::NONE) {
}

void DifficultyMenuState::Enter()
{
	if (!m_renderer) return;

	m_scene = new DifficultyScene();
	if (m_scene) m_scene->Initialise(*m_renderer);
}

void DifficultyMenuState::Update(float deltatime)
{
	if (m_scene && m_inputSystem) {
		m_scene->Process(deltatime, *m_inputSystem);
        if (m_scene->Home()) {
            m_nextState = GameStates::START_MENU;
        }

		if (m_scene->GetStatus()) {
			char selection = m_scene->GetSelection();

            switch (selection) {
            case 'E':
                m_nextState = GameStates::GAMEPLAY;
                m_gameDifficulty = GameDifficulty::EASY;
                break;
            case 'N':
                m_nextState = GameStates::GAMEPLAY;
                m_gameDifficulty = GameDifficulty::NORMAL;
                break;
            case 'H':
                m_nextState = GameStates::GAMEPLAY;
                m_gameDifficulty = GameDifficulty::HARD;
                break;
            default:
                m_nextState = GameStates::NONE;
                break;
            }
		}
	}
}

void DifficultyMenuState::Draw()
{
	if (m_scene && m_renderer) m_scene->Draw(*m_renderer);
}

void DifficultyMenuState::DebugDraw()
{
	if (m_scene) m_scene->DebugDraw();
}

void DifficultyMenuState::Exit()
{
	delete m_scene;
	m_scene = 0;
    m_nextState = GameStates::NONE;
}

GameStates DifficultyMenuState::GetNextState() const
{
	return m_nextState;
}
