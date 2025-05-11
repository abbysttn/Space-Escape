#include "leveltransitionstate.h"

#include "leveltransition.h"

LevelTransitionState::LevelTransitionState() : m_scene(nullptr), m_nextState(GameStates::NONE) {}

void LevelTransitionState::Enter()
{
	if (!m_renderer) return;

	m_scene = new LevelTransition();
	if (m_scene) m_scene->Initialise(*m_renderer);
}

void LevelTransitionState::Update(float deltatime)
{
	if (m_scene && m_inputSystem) {
		m_scene->Process(deltatime, *m_inputSystem);
		if (m_scene->GetStatus()) {
			m_nextState = GameStates::GAMEPLAY;
		}
	}
}

void LevelTransitionState::Draw()
{
	if (m_scene && m_renderer) m_scene->Draw(*m_renderer);
}

void LevelTransitionState::DebugDraw()
{
	if (m_scene) m_scene->DebugDraw();
}

void LevelTransitionState::Exit()
{
	delete m_scene;
	m_scene = 0;
}

GameStates LevelTransitionState::GetNextState() const
{
	return m_nextState;
}
