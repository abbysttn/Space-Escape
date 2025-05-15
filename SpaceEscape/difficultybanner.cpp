#include "difficultybanner.h"

#include "textrenderer.h"
#include "sprite.h"
#include "Renderer.h"
#include "inputsystem.h"

DifficultyBanner::DifficultyBanner(int effect, char diff, int level) : m_banner(0), m_difficulty(0), m_planetEffect(0),
difficultyChar(diff), planetEffectNumber(effect), m_sceneDone(false), m_timer(0.0f), m_maxTime(3.0f), m_levelNumber(level) {}

DifficultyBanner::~DifficultyBanner()
{
	delete m_difficulty;
	delete m_planetEffect;
	delete m_banner;
	m_banner = 0;
}

bool DifficultyBanner::Initialise(Renderer& renderer)
{
	screenOffsetX = (float)renderer.GetWidth();
	screenOffsetY = (float)renderer.GetHeight();

	planetEffect = GetPlanetEffect(planetEffectNumber);
	difficulty = GetDifficulty(difficultyChar);

	m_difficulty = new TextRenderer();
	m_difficulty->initialise(renderer, difficulty.c_str(), 150);

	m_planetEffect = new TextRenderer();
	m_planetEffect->initialise(renderer, planetEffect.c_str(), 100);

	m_banner = renderer.CreateSprite("..\\assets\\banner.png");
	m_banner->SetX((int)screenOffsetX / 2);
	m_banner->SetY((int)screenOffsetY / 2);

	//full width
	float scaleFactor = screenOffsetX / (float)m_banner->GetWidth();

	m_banner->SetScale(scaleFactor);

	Vector2 textPos((float)m_banner->GetX(), (float)m_banner->GetY());

	m_difficulty->Position() = textPos;
	m_difficulty->Position().y -= m_difficulty->GetSpriteHeight() / 2.0f;
	m_planetEffect->Position() = textPos;
	m_planetEffect->Position().y += m_planetEffect->GetSpriteHeight() / 2.0f;

	return false;
}

void DifficultyBanner::Process(float deltaTime, InputSystem& inputSystem)
{
	if (m_timer < m_maxTime) {
		m_timer += deltaTime;
	}

	if (m_timer >= m_maxTime) {
		m_sceneDone = true;
	}

	m_difficulty->Process(deltaTime);
	m_planetEffect->Process(deltaTime);
	m_banner->Process(deltaTime);
}

void DifficultyBanner::Draw(Renderer& renderer)
{
	m_banner->Draw(renderer);
	m_difficulty->Draw(renderer);
	m_planetEffect->Draw(renderer);
}

void DifficultyBanner::DebugDraw()
{
}

bool DifficultyBanner::GetStatus()
{
	return m_sceneDone;
}

string DifficultyBanner::GetPlanetEffect(int& planetEffectNumber)
{
	string effect;
	switch (planetEffectNumber) {
	case 0:
		//default or normal effect
		effect = "Normal Planet";
		break;

	case 1:
		//slow time
		effect = "Low Gravity Planet";
		break;

	case 2:
		//reverse
		effect = "Backwards Planet";
		break;

	case 3:
		//fast time
		effect = "High Gravity Planet";
		break;
	}

	return effect;
}

string DifficultyBanner::GetDifficulty(char& difficultyChar)
{
	string diff;

	switch (difficultyChar) {
	case 'E':
		diff = "Easy";
		break;

	case 'M':
		diff = "Medium";
		break;

	case 'H':
		if (m_levelNumber == 5) {
			diff = "Boss Level";
		}
		else {
			diff = "Hard";
		}
		break;

	}
	return diff;
}
