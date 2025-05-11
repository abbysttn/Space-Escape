#include "leveltransition.h"

#include "Renderer.h"
#include "player.h"
#include "sprite.h"
#include "inputsystem.h"

LevelTransition::LevelTransition() : m_player(0), m_blackHole(0), m_sceneDone(false) {}

LevelTransition::~LevelTransition()
{
	delete m_player;
	m_player = 0;

	delete m_blackHole;
	m_blackHole = 0;
}

bool LevelTransition::Initialise(Renderer& renderer)
{
	int screenWidth = renderer.GetWidth();
	int screenHeight = renderer.GetHeight();

	m_player = new Player();
	m_player->Initialise(renderer, "..\\assets\\run_right.png");
	m_player->SetActive(true);
	m_player->SetLoop(true);
	m_player->SetFrameTime(0.05f);
	m_player->Position().x = screenWidth / 2.0f;
	m_player->Position().y = screenHeight;

	m_startPos = Vector2(m_player->Position().x, m_player->Position().y);

	m_blackHole = renderer.CreateSprite("..\\assets\\black_hole.png");
	m_blackHole->SetX(screenWidth / 2.0f);
	m_blackHole->SetY(screenHeight / 2.0f);
	m_blackHole->SetScale(6.0f);

	m_swirlProgress = 0.0f;
	m_swirlTime = 3.0f;

	GenerateFibonacciSpiral(100, screenWidth / 2.0f, screenHeight / 2.0f);

	return true;
}

void LevelTransition::Process(float deltaTime, InputSystem& inputSystem)
{
	if (m_swirlProgress < 1.0f) {
		m_swirlProgress += deltaTime / m_swirlTime;

		if (m_swirlProgress > 1.0f) m_swirlProgress = 1.0f;

		AnimatePlayerSwirl();
	}
	else {
		m_sceneDone = true;
	}

	m_player->Process(deltaTime);
	m_blackHole->Process(deltaTime);
}

void LevelTransition::Draw(Renderer& renderer)
{
	renderer.SetClearColour(13, 16, 28);

	m_blackHole->Draw(renderer);
	m_player->Draw(renderer);
}

void LevelTransition::DebugDraw()
{
}

bool LevelTransition::GetStatus()
{
	return m_sceneDone;
}

void LevelTransition::GenerateFibonacciSpiral(int points, float centerX, float centerY)
{
	m_fibonacciPoints.clear();

	float startDist = sqrt(pow(m_player->Position().x - centerX, 2) + pow(m_player->Position().y - centerY, 2));

	for (int i = 0; i <= points; i++) {
		float progress = (float)i / points;
		float angle = progress * 10 * 3.14159f;
		float radius = startDist * (1 - progress * 0.9f);

		Vector2 point;
		point.x = centerX + radius * cos(angle);
		point.y = centerY + radius * sin(angle);

		if (i == 0) {
			m_player->Position() = point;
		}

		m_fibonacciPoints.push_back(point);
	}

	m_fibonacciPoints.back() = Vector2(centerX, centerY);
}

void LevelTransition::AnimatePlayerSwirl()
{
	if (m_fibonacciPoints.empty()) return;

	float easedProgress = m_swirlProgress * m_swirlProgress * (3 - 2 * m_swirlProgress);
	float segment = easedProgress * (m_fibonacciPoints.size() - 1);

	int fib1 = (int)segment;
	int fib2;

	if (fib1 + 1 < (int)(m_fibonacciPoints.size() - 1))	fib2 = fib1 + 1;
	else fib2 = (int)(m_fibonacciPoints.size() - 1);

	float t = segment - fib1;

	Vector2 pos;
	pos.x = Linear(m_fibonacciPoints[fib1].x, m_fibonacciPoints[fib2].x, t);
	pos.y = Linear(m_fibonacciPoints[fib1].y, m_fibonacciPoints[fib2].y, t);

	m_player->Position() = pos;

	if (fib2 < m_fibonacciPoints.size() - 1) {
		Vector2 dir = m_fibonacciPoints[fib2] - m_fibonacciPoints[fib1];
		float angle = atan2(dir.y, dir.x);
		m_player->SetRotation(angle);
	}

	float scale = 7.0f - (m_swirlProgress * 7.0f);
	m_player->SetPlayerScale(scale);
}

float LevelTransition::Linear(float a, float b, float t)
{
	return a + t * (b - a);
}
