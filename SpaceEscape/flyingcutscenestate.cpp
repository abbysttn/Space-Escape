#include "flyingcutscenestate.h"

#include "flyingcutscene.h"

FlyingCutsceneState::FlyingCutsceneState() : m_scene(nullptr), m_nextState(GameStates::NONE) {}

void FlyingCutsceneState::Enter()
{
	if (!m_renderer) return;

	m_scene = new FlyingCutscene();
	if (m_scene) m_scene->Initialise(*m_renderer);
}

void FlyingCutsceneState::Update(float deltatime)
{
	if (m_scene && m_inputSystem) {
		m_scene->Process(deltatime, *m_inputSystem);
		if (m_scene->GetStatus()) {
			m_nextState = GameStates::START_MENU;
		}
	}
}

void FlyingCutsceneState::Draw()
{
	if (m_scene && m_renderer) m_scene->Draw(*m_renderer);
}

void FlyingCutsceneState::DebugDraw()
{
	if (m_scene) m_scene->DebugDraw();
}

void FlyingCutsceneState::Exit()
{
	delete m_scene;
	m_scene = 0;
	m_nextState = GameStates::NONE;
}

GameStates FlyingCutsceneState::GetNextState() const
{
	return m_nextState;
}
