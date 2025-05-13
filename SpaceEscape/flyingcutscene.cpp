#include "flyingcutscene.h"

#include "Renderer.h"
#include "inputsystem.h"
#include "playership.h"

FlyingCutscene::FlyingCutscene() : m_ship(0), m_sceneDone(false) {}

FlyingCutscene::~FlyingCutscene()
{
	delete m_ship;
}

bool FlyingCutscene::Initialise(Renderer& renderer)
{
	m_ship = new PlayerShip();
	m_ship->Initialise(renderer);
	m_ship->SetActive(true);
	m_ship->Position().y = 10.0f;

	//add particles

	return true;
}

void FlyingCutscene::Process(float deltaTime, InputSystem& inputSystem)
{
	m_ship->Fall(deltaTime);
	m_ship->Process(deltaTime);
}

void FlyingCutscene::Draw(Renderer& renderer)
{
	renderer.SetClearColour(13, 16, 28);
	m_ship->Draw(renderer);
}

void FlyingCutscene::DebugDraw()
{
}

bool FlyingCutscene::GetStatus()
{
	if (m_ship->IsActive()) {
		return false;
	}
	return true;
}
