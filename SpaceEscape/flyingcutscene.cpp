#include "flyingcutscene.h"

#include "Renderer.h"
#include "inputsystem.h"
#include "player.h"
#include "sprite.h"

FlyingCutscene::FlyingCutscene() : m_player(0), m_ship(0), m_sceneDone(false) {}

FlyingCutscene::~FlyingCutscene()
{
	delete m_player;
	delete m_ship;
	m_ship = 0;
}

bool FlyingCutscene::Initialise(Renderer& renderer)
{
	return false;
}

void FlyingCutscene::Process(float deltaTime, InputSystem& inputSystem)
{
}

void FlyingCutscene::Draw(Renderer& renderer)
{
}

void FlyingCutscene::DebugDraw()
{
}

bool FlyingCutscene::GetStatus()
{
	return false;
}
