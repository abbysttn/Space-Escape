#include "splashautstate.h"

#include "scenesplashfmod.h"

void SplashAUTState::Enter()
{
	m_scene = new SceneSplashFMOD();
	m_scene->Initialise(*m_renderer);
}

void SplashAUTState::Update()
{
	//m_scene->Process()
	if (m_scene->getStatus()) {
		m_nextState = GameStates::SPLASH_FMOD;
	}
}

void SplashAUTState::Exit()
{
	delete m_scene;
	m_scene = 0;
}

GameStates SplashAUTState::GetNextState() const
{
	return m_nextState;
}
