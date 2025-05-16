#include "flyingcutscene.h"

#include "Renderer.h"
#include "inputsystem.h"
#include "playership.h"
#include "textrenderer.h"

#include "particleemitter.h"

FlyingCutscene::FlyingCutscene() : m_ship(0), m_sceneDone(false), m_exhaustEmitter(0), m_gameMessage1(0), m_gameMessage2(0), m_textTime(3.0f), m_textTimer(0.0f) {}

FlyingCutscene::~FlyingCutscene()
{
	delete m_ship;
	delete m_exhaustEmitter;
	delete m_gameMessage1;
	delete m_gameMessage2;
}

bool FlyingCutscene::Initialise(Renderer& renderer)
{
	float centerX = renderer.GetWidth() / 2.0f;
	float centerY = renderer.GetHeight() / 2.0f;

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

	m_gameMessage1 = new TextRenderer();
	m_gameMessage1->initialise(renderer, "Collect The Ship Parts", 100);
	m_gameMessage1->Position() = { centerX, centerY - m_gameMessage1->GetSpriteHeight() };

	m_gameMessage2 = new TextRenderer();
	m_gameMessage2->initialise(renderer, "To Survive The Rift", 100);
	m_gameMessage2->Position() = { centerX, centerY + m_gameMessage1->GetSpriteHeight() };

	return true;
}

void FlyingCutscene::Process(float deltaTime, InputSystem& inputSystem)
{
	if (m_textTimer < m_textTime) {
		m_textTimer += deltaTime;
	}

	if (m_textTimer >= m_textTime) {
		m_sceneDone = true;
	}

	m_ship->Fall(deltaTime);
	m_exhaustEmitter->SetPosition(m_ship->Position().x, m_ship->Position().y + m_particleOffset);
	m_exhaustEmitter->Process(deltaTime);
	m_ship->Process(deltaTime);
	m_gameMessage1->Process(deltaTime);
	m_gameMessage2->Process(deltaTime);
}

void FlyingCutscene::Draw(Renderer& renderer)
{
	renderer.SetClearColour(13, 16, 28);
	m_ship->Draw(renderer);
	m_exhaustEmitter->Draw(renderer);

	if (!m_ship->IsActive()) {
		m_gameMessage1->Draw(renderer);
		m_gameMessage2->Draw(renderer);
	}
	else {
		m_textTimer = 0.0f;
	}
}

void FlyingCutscene::DebugDraw()
{
}

bool FlyingCutscene::GetStatus()
{
	return m_sceneDone;
}
