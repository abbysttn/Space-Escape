#include "startscene.h"

#include "Renderer.h"
#include "inputsystem.h"
#include "sprite.h"

StartScene::StartScene() : m_pText(0), m_button(0) {}

StartScene::~StartScene()
{
	delete m_pText;
	m_pText = 0;

	delete m_button;
	m_button = 0;
}

bool StartScene::Initialise(Renderer& renderer)
{
	m_button = renderer.CreateSprite("..\\assets\\arrow_left.png");

	renderer.CreateStaticText("Space Escape", 100);
	m_pText = renderer.CreateSprite("Space Escape");
	if (!m_pText) return false;

	
	//if (!m_button) return false;

	int screenWidth = renderer.GetWidth();
	int screenHeight = renderer.GetHeight();

	int textHeight = m_pText->GetHeight();

	float centeredX = screenWidth / 2.0f; // center x
	float centeredY = (screenHeight - textHeight) / 2.0f; // center y

	m_pText->SetX(centeredX);
	m_pText->SetY(centeredY);
	m_pText->SetAlpha(1.0f);

	m_button->SetX(centeredX);
	m_button->SetY(centeredY);
	m_button->SetScale(3.0f);

	return true;
}

void StartScene::Process(float deltaTime, InputSystem& inputSystem)
{
	m_pText->Process(deltaTime);
	m_button->Process(deltaTime);
}

void StartScene::Draw(Renderer& renderer)
{
	renderer.SetClearColour(71, 171, 169);
	m_pText->Draw(renderer);
	m_button->Draw(renderer);
}

void StartScene::DebugDraw()
{
}
