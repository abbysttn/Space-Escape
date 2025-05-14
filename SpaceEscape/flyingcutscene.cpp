#include "flyingcutscene.h"

#include "Renderer.h"
#include "inputsystem.h"
#include "playership.h"

#include "particleemitter.h"

FlyingCutscene::FlyingCutscene() : m_ship(0), m_sceneDone(false), m_exhaustEmitter(0) {}

FlyingCutscene::~FlyingCutscene()
{
	delete m_ship;
	delete m_exhaustEmitter;
}

bool FlyingCutscene::Initialise(Renderer& renderer)
{
	m_ship = new PlayerShip();
	m_ship->Initialise(renderer);
	m_ship->SetActive(true);
	m_ship->Position().y = 10.0f;

	m_exhaustEmitter = new ParticleEmitter();
	m_exhaustEmitter->Initialise(renderer, "..\\assets\\smoke.png");

	float colour[3] = { 0.69f, 0.22f, 0.09f };

	m_exhaustEmitter->SetColour(colour);
	m_exhaustEmitter->SetAngles(359.0f, 0.0f);
	m_exhaustEmitter->SetEmitRate(0.1f);
	m_exhaustEmitter->SetBatchSize(5);
	m_exhaustEmitter->SetLifeSpan(1.0f);
	m_exhaustEmitter->SetAcceleration(180.0f);
	m_exhaustEmitter->SetActive(true);

	m_particleOffset = m_ship->GetSpriteWidth() / 3.0f;

	return true;
}

void FlyingCutscene::Process(float deltaTime, InputSystem& inputSystem)
{
	m_ship->Fall(deltaTime);
	m_exhaustEmitter->SetPosition(m_ship->Position().x, m_ship->Position().y + m_particleOffset);
	m_exhaustEmitter->Process(deltaTime);
	m_ship->Process(deltaTime);
}

void FlyingCutscene::Draw(Renderer& renderer)
{
	renderer.SetClearColour(13, 16, 28);
	m_ship->Draw(renderer);
	m_exhaustEmitter->Draw(renderer);
}

void FlyingCutscene::DebugDraw()
{
}

bool FlyingCutscene::GetStatus()
{
	if (!m_ship->IsActive()) {
		m_sceneDone = true;
	}

	return m_sceneDone;
}
