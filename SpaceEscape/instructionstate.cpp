#include "instructionstate.h"

#include "instructionscene.h"

InstructionState::InstructionState() : m_scene(nullptr), m_nextState(GameStates::NONE) {}

void InstructionState::Enter()
{
	if (!m_renderer) return;

	m_scene = new InstructionScene();
	if (m_scene) m_scene->Initialise(*m_renderer);
}

void InstructionState::Update(float deltatime)
{
    if (m_scene && m_inputSystem) {
        m_scene->Process(deltatime, *m_inputSystem);
		if (m_scene->Home()) {
			m_nextState = GameStates::START_MENU;
		}
    }
}

void InstructionState::Draw()
{
	if (m_scene && m_renderer) m_scene->Draw(*m_renderer);
}

void InstructionState::DebugDraw()
{
	if (m_scene) m_scene->DebugDraw();
}

void InstructionState::Exit()
{
	delete m_scene;
	m_scene = 0;
	m_nextState = GameStates::NONE;
}

GameStates InstructionState::GetNextState() const
{
	return m_nextState;
}
