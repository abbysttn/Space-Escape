#include "scenesplashfmod.h"

#include "Renderer.h"
#include "sprite.h"
#include "particleemitter.h"

#include <cassert>

SceneSplashFMOD::SceneSplashFMOD() : m_pFMOD(0), m_alpha(0.0f) {}

SceneSplashFMOD::~SceneSplashFMOD()
{
	delete m_pFMOD;
	m_pFMOD = 0;
}

bool SceneSplashFMOD::Initialise(Renderer& renderer)
{
	m_pFMOD = renderer.CreateSprite("..\\assets\\FMOD Logo White - Black Background.png");

	const int halfSpriteW = m_pFMOD->GetWidth() / 2;
	const int halfSpriteH = m_pFMOD->GetHeight() / 2;

	const int screenW = renderer.GetWidth();
	const int screenH = renderer.GetHeight();

	m_pFMOD->SetX(screenW / 2);
	m_pFMOD->SetY(screenH / 2);

	return true;
}

void SceneSplashFMOD::Process(float deltaTime, InputSystem& inputSystem)
{
	if (increase) {
		m_alpha += 0.3f * deltaTime;
		if (m_alpha >= 1.0f) {
			m_alpha = 1.0f;
			increase = false;
		}
	}
	else {
		m_alpha -= 0.3f * deltaTime;
		if (m_alpha <= 0.0f) {
			m_alpha = 0.0f;
			done = true;
		}
	}

	m_pFMOD->SetAlpha(m_alpha);
	m_pFMOD->Process(deltaTime);
}

void SceneSplashFMOD::Draw(Renderer& renderer)
{
	renderer.SetClearColour(0, 0, 0);
	m_pFMOD->Draw(renderer);
}

void SceneSplashFMOD::DebugDraw()
{
}

bool SceneSplashFMOD::getStatus()
{
	return done;
}
