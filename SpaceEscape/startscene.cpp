#include "startscene.h"

#include "Renderer.h"
#include "inputsystem.h"
#include "sprite.h"

#include "textrenderer.h"
#include "gameobjectpool.h"

StartScene::StartScene() : m_button(0), m_textPool(nullptr) {}

StartScene::~StartScene()
{
	delete m_button;
	m_button = 0;
}

bool StartScene::Initialise(Renderer& renderer)
{
	m_button = renderer.CreateSprite("..\\assets\\button_normal_long.png");

	m_textPool = new GameObjectPool(TextRenderer(), 2);

	int screenWidth = renderer.GetWidth();
	int screenHeight = renderer.GetHeight();

	int yOffset = 0;

	for (size_t i = 0; i < m_textPool->totalCount(); i++) {
		if (GameObject* obj = m_textPool->getObjectAtIndex(i)) {
			TextRenderer* text = dynamic_cast<TextRenderer*>(obj);
			if (i == 0) text->initialise(renderer, "Space Escape", 150);
			else text->initialise(renderer, "Start", 100);
			int textHeight = text->GetSpriteHeight();
			if (i > 0) yOffset -= textHeight * 4;

			float centeredX = screenWidth / 2.0f; // center x
			float textY = ((screenHeight - yOffset) / 2.0f) - 300.0f;

			text->Position().x = centeredX;
			text->Position().y = textY;
		}
	}

	yOffset /= 2;

	float centeredX = screenWidth / 2.0f; // center x
	float centeredY = (screenHeight - yOffset) / 2.0f; // center y

	m_button->SetX(centeredX);
	m_button->SetY(centeredY);
	m_button->SetScale(6.0f);

	return true;
}

void StartScene::Process(float deltaTime, InputSystem& inputSystem)
{
	for (size_t i = 0; i < m_textPool->totalCount(); i++) {
		if (GameObject* obj = m_textPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<TextRenderer*>(obj)) {
				TextRenderer* text = static_cast<TextRenderer*>(obj);
				text->Process(deltaTime);
			}
		}
	}

	m_button->Process(deltaTime);
}

void StartScene::Draw(Renderer& renderer)
{
	renderer.SetClearColour(0, 0, 0);
	
	for (size_t i = 0; i < m_textPool->totalCount(); i++) {
		if (GameObject* obj = m_textPool->getObjectAtIndex(i)) {
			if (obj && obj->isActive()) {
				TextRenderer* text = static_cast<TextRenderer*>(obj);
				text->Draw(renderer);
			}
		}
	}

	m_button->Draw(renderer);
}

void StartScene::DebugDraw()
{
}
