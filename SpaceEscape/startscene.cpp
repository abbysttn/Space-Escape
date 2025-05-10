#include "startscene.h"

#include "Renderer.h"
#include "inputsystem.h"
#include "sprite.h"

#include "textrenderer.h"
#include "button.h"
#include "selectionarrow.h"
#include "gameobjectpool.h"

#include "logmanager.h"

#include "quadtree.h"

StartScene::StartScene() : m_textPool(nullptr), m_buttonPool(nullptr), m_arrowPool(nullptr), m_backgroundPlanet(0) {}

StartScene::~StartScene()
{
	delete m_textPool;
	delete m_buttonPool;
	delete m_arrowPool;

	delete m_backgroundPlanet;
	m_backgroundPlanet = 0;
}

bool StartScene::Initialise(Renderer& renderer)
{
	m_textPool = new GameObjectPool(TextRenderer(), 4);
	m_buttonPool = new GameObjectPool(Button(), 3);
	m_arrowPool = new GameObjectPool(SelectionArrow(), 2);

	int screenWidth = renderer.GetWidth();
	int screenHeight = renderer.GetHeight();

	int yOffset = 0;

	int buttonIndex = 0;

	m_backgroundPlanet = renderer.CreateSprite("..\\assets\\Lava.png");
	m_backgroundPlanet->SetScale(10.0f);
	m_backgroundPlanet->SetX(screenWidth / 2.0f);
	m_backgroundPlanet->SetY(screenHeight / 2.0f);

	for (size_t i = 0; i < m_buttonPool->totalCount(); i++) {
		if (GameObject* obj = m_buttonPool->getObjectAtIndex(i)) {
			Button* button = dynamic_cast<Button*>(obj);
			
			button->initialise(renderer);
		}
	}

	for (size_t i = 0; i < m_arrowPool->totalCount(); i++) {
		if (GameObject* obj = m_arrowPool->getObjectAtIndex(i)) {
			SelectionArrow* arrow = dynamic_cast<SelectionArrow*>(obj);

			char direction;

			if (i % 2 == 0) {
				direction = 'L';
			}
			else {
				direction = 'R';
			}

			arrow->initialise(renderer, direction);
		}
	}

	for (size_t i = 0; i < m_textPool->totalCount(); i++) {
		if (GameObject* obj = m_textPool->getObjectAtIndex(i)) {
			TextRenderer* text = dynamic_cast<TextRenderer*>(obj);

			switch (i) {
			case 0:
				text->initialise(renderer, "Space Escape", 150);
				break;
			case 1:
				text->initialise(renderer, "Start", 90);
				break;
			case 2:
				text->initialise(renderer, "How To Play", 90);
				break;
			case 3:
				text->initialise(renderer, "Exit", 90);
				break;
			default:
				break;
			}

			int textHeight = text->GetSpriteHeight();

			if (i > 0) yOffset -= textHeight * 4;

			float centeredX = screenWidth / 2.0f;
			float textY = ((screenHeight - yOffset) / 2.0f) - 300.0f;

			text->Position().x = centeredX;
			text->Position().y = textY;

			if (i > 0) {
				if (GameObject* obj = m_buttonPool->getObjectAtIndex(i - 1)) {
					Button* button = dynamic_cast<Button*>(obj);

					button->Position() = text->Position();

					if (i == 1) {
						for (size_t j = 0; j < m_arrowPool->totalCount(); j++) {
							if (GameObject* obj = m_arrowPool->getObjectAtIndex(j)) {
								SelectionArrow* arrow = dynamic_cast<SelectionArrow*>(obj);

								arrow->Position() = button->Position();
								arrow->SetOffset((float)button->GetSpriteWidth());
							}
						}
					}
				}

				text->Position().y -= 8.0f;
			}
		}
	}

	m_collisionTree = make_unique<QuadTree>(Box(0.0f, 0.0f, (float)renderer.GetWidth(), (float)renderer.GetHeight()));

	return true;
}

void StartScene::Process(float deltaTime, InputSystem& inputSystem)
{
	m_collisionTree->clear();

	m_backgroundPlanet->Process(deltaTime);

	for (size_t i = 0; i < m_textPool->totalCount(); i++) {
		if (GameObject* obj = m_textPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<TextRenderer*>(obj)) {
				TextRenderer* text = static_cast<TextRenderer*>(obj);
				text->Process(deltaTime);
			}
		}
	}

	for (size_t i = 0; i < m_buttonPool->totalCount(); i++) {
		if (GameObject* obj = m_buttonPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<Button*>(obj)) {
				Button* button = static_cast<Button*>(obj);
				button->Process(deltaTime);

				Box buttonRange(
					button->Position().x - button->GetSpriteWidth() / 2,
					button->Position().y - button->GetSpriteHeight() / 2,
					button->GetSpriteWidth(),
					button->GetSpriteHeight()
				);

				m_collisionTree->insert(button, buttonRange);
			}
		}
	}

	for (size_t i = 0; i < m_arrowPool->totalCount(); i++) {
		if (GameObject* obj = m_arrowPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<SelectionArrow*>(obj)) {
				SelectionArrow* arrow = static_cast<SelectionArrow*>(obj);
				arrow->Process(deltaTime);
			}
		}
	}

	bool clicked = CheckMousePos(&inputSystem);
}

void StartScene::Draw(Renderer& renderer)
{
	renderer.SetClearColour(0, 0, 0);

	m_backgroundPlanet->Draw(renderer);

	for (size_t i = 0; i < m_buttonPool->totalCount(); i++) {
		if (GameObject* obj = m_buttonPool->getObjectAtIndex(i)) {
			if (obj && obj->isActive()) {
				Button* button = static_cast<Button*>(obj);
				button->Draw(renderer);
			}
		}
	}

	for (size_t i = 0; i < m_arrowPool->totalCount(); i++) {
		if (GameObject* obj = m_arrowPool->getObjectAtIndex(i)) {
			if (obj && obj->isActive()) {
				SelectionArrow* arrow = static_cast<SelectionArrow*>(obj);
				arrow->Draw(renderer);
			}
		}
	}
	
	for (size_t i = 0; i < m_textPool->totalCount(); i++) {
		if (GameObject* obj = m_textPool->getObjectAtIndex(i)) {
			if (obj && obj->isActive()) {
				TextRenderer* text = static_cast<TextRenderer*>(obj);
				text->Draw(renderer);
			}
		}
	}	
}

void StartScene::DebugDraw()
{
}

bool StartScene::IsColliding(const Box& box, Button* button)
{
	if (!button) return false;

	float bL = button->Position().x - button->GetSpriteWidth() / 2;
	float bR = bL + button->GetSpriteWidth();
	float bT = button->Position().y - button->GetSpriteHeight() / 2;
	float bB = bT + button->GetSpriteHeight();

	return !(box.x + box.width < bL ||
		box.x > bR ||
		box.y + box.height < bT ||
		box.y > bB);
}

bool StartScene::CheckMousePos(InputSystem* inputSystem)
{
	Vector2 mousePos = inputSystem->GetMousePosition();
	bool mouseClicked = inputSystem->GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED;

	Box mouseBox(
		mousePos.x - 5.0f, mousePos.y - 5.0f,
		10.0f, 10.0f);

	auto potentialCollisions = m_collisionTree->queryRange(mouseBox);

	for (auto* obj : potentialCollisions) {
		Button* button = dynamic_cast<Button*>(obj);
		if (button && IsColliding(mouseBox, button)) {
			for (size_t i = 0; i < m_arrowPool->totalCount(); i++) {
				if (GameObject* obj = m_arrowPool->getObjectAtIndex(i)) {
					if (obj && dynamic_cast<SelectionArrow*>(obj)) {
						SelectionArrow* arrow = static_cast<SelectionArrow*>(obj);
						arrow->Position() = button->Position();
						arrow->SetOffset((float)button->GetSpriteWidth());
						if (mouseClicked) {
							button->Pressed();
							arrow->Pressed();
						}
					}
				}
			}
			break;
		}
	}

	return true;
}
