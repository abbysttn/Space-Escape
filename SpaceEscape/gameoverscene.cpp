#include "gameoverscene.h"

#include "Renderer.h"
#include "inputsystem.h"
#include "playership.h"
#include "textrenderer.h"

#include "particleemitter.h"

GameOverScene::GameOverScene(bool won) : m_sceneDone(false), m_ship(0), m_gameMessage(0), m_gameWon(won), m_exhaustEmitter(0) {}

GameOverScene::~GameOverScene()
{
	delete m_ship;
	delete m_gameMessage;
	delete m_exhaustEmitter;
}

bool GameOverScene::Initialise(Renderer& renderer)
{
	float screenWidth = (float)renderer.GetWidth();
	float screenHeight = (float)renderer.GetHeight();

	m_ship = new PlayerShip();
	m_ship->Initialise(renderer);
	m_ship->SetActive(true);

	m_exhaustEmitter = new ParticleEmitter();
	m_exhaustEmitter->Initialise(renderer, "..\\assets\\smoke.png");

	m_gameMessage = new TextRenderer();

	if (m_gameWon) {
		m_gameMessage->initialise(renderer, "You Survived The Rift!", 100);

		float colour[3] = { 0.31f, 0.25f, 0.22f };

		m_exhaustEmitter->SetColour(colour);
		m_exhaustEmitter->SetAngles(330.0f, 30.0f);
		m_exhaustEmitter->SetEmitRate(0.1f);
		m_exhaustEmitter->SetBatchSize(3);
		m_exhaustEmitter->SetLifeSpan(0.8f);
		m_exhaustEmitter->SetAcceleration(120.0f);
	}
	else {
		m_gameMessage->initialise(renderer, "Lost To The Rift...", 100);
		m_ship->Position().y = 0.0f;

		float colour[3] = { 0.69f, 0.22f, 0.09f };

		m_exhaustEmitter->SetColour(colour);
		m_exhaustEmitter->SetAngles(359.0f, 0.0f);
		m_exhaustEmitter->SetEmitRate(0.1f);
		m_exhaustEmitter->SetBatchSize(5);
		m_exhaustEmitter->SetLifeSpan(1.0f);
		m_exhaustEmitter->SetAcceleration(180.0f);
	}

	m_particleOffset = m_ship->GetSpriteWidth() / 3.0f;

	m_gameMessage->Position() = { screenWidth / 2.0f, screenHeight / 2.0f };

	return true;
}

void GameOverScene::Process(float deltaTime, InputSystem& inputSystem)
{
	if (m_gameWon) {
		m_ship->Fly(deltaTime);
		m_exhaustEmitter->SetPosition(m_ship->Position().x - m_particleOffset, m_ship->Position().y);
	}
	else {
		m_ship->Fall(deltaTime);
		m_exhaustEmitter->SetPosition(m_ship->Position().x, m_ship->Position().y + m_particleOffset);
	}
	
	m_ship->Process(deltaTime);
	m_gameMessage->Process(deltaTime);
	m_exhaustEmitter->Process(deltaTime);
}

void GameOverScene::Draw(Renderer& renderer)
{
	renderer.SetClearColour(13, 16, 28);
	m_gameMessage->Draw(renderer);
	if (m_gameWon) {
		m_exhaustEmitter->Draw(renderer);
	}
	m_ship->Draw(renderer);
	if (!m_gameWon) {
		m_exhaustEmitter->Draw(renderer);
	}
	
}

void GameOverScene::DebugDraw()
{
}

bool GameOverScene::GetStatus()
{
	if (!m_ship->IsActive()) {
		m_sceneDone = true;
	}

	return m_sceneDone;
}
