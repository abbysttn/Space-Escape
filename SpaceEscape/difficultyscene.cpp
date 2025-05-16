#include "difficultyscene.h"

#include "Renderer.h"
#include "inputsystem.h"
#include "sprite.h"

#include "textrenderer.h"
#include "button.h"
#include "selectionarrow.h"
#include "gameobjectpool.h"
#include "xboxcontroller.h"

#include "homebutton.h"

#include "logmanager.h"

#include "quadtree.h"

DifficultyScene::DifficultyScene() : m_textPool(nullptr), m_buttonPool(nullptr), m_arrowPool(nullptr), m_backgroundPlanet(0), m_sceneDone(false), m_currentSelectIndex(0)
, m_selected(false), m_xboxUsed(false), m_homeButton(0), m_home(false) {}

DifficultyScene::~DifficultyScene()
{
	delete m_textPool;
	delete m_buttonPool;
	delete m_arrowPool;
	delete m_homeButton;

	delete m_backgroundPlanet;
	m_backgroundPlanet = 0;
}

bool DifficultyScene::Initialise(Renderer& renderer)
{
	m_textPool = new GameObjectPool(TextRenderer(), 4);
	m_buttonPool = new GameObjectPool(Button(), 3);
	m_arrowPool = new GameObjectPool(SelectionArrow(), 2);

	int screenWidth = renderer.GetWidth();
	int screenHeight = renderer.GetHeight();

	int yOffset = 0;

	int buttonIndex = 0;

	m_backgroundPlanet = renderer.CreateSprite("..\\assets\\terran.png");
	m_backgroundPlanet->SetScale(10.0f);
	m_backgroundPlanet->SetX(screenWidth / 2);
	m_backgroundPlanet->SetY(screenHeight / 2);

	m_homeButton = new HomeButton();
	m_homeButton->initialise(renderer);
	m_homeButton->Position() = { screenWidth - 48.0f, screenHeight - 48.0f };

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
				text->initialise(renderer, "Difficulty", 150);
				break;
			case 1:
				text->initialise(renderer, "Easy", 90);
				break;
			case 2:
				text->initialise(renderer, "Normal", 90);
				break;
			case 3:
				text->initialise(renderer, "Hard", 90);
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

void DifficultyScene::Process(float deltaTime, InputSystem& inputSystem)
{
	m_collisionTree->clear();

	m_backgroundPlanet->Process(deltaTime);

	m_homeButton->Process(deltaTime);
	Box homeRange(
		m_homeButton->Position().x - m_homeButton->GetSpriteWidth() / 2,
		m_homeButton->Position().y - m_homeButton->GetSpriteHeight() / 2,
		(float)m_homeButton->GetSpriteWidth(),
		(float)m_homeButton->GetSpriteHeight()
	);

	m_collisionTree->insert(m_homeButton, homeRange);

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
					(float)button->GetSpriteWidth(),
					(float)button->GetSpriteHeight()
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

	if (inputSystem.GetNumberOfControllersAttached() > 0) {

		XboxController* controller = new XboxController();
		controller = inputSystem.GetController(0);

		if (controller) {
			if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_DOWN) == BS_PRESSED) {
				m_xboxUsed = true;
				m_currentSelectIndex++;
				if (m_currentSelectIndex >= (int)m_buttonPool->totalCount()) {
					m_currentSelectIndex = 0;
				}
			}
			else if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_UP) == BS_PRESSED) {
				m_xboxUsed = true;
				m_currentSelectIndex--;
				if (m_currentSelectIndex < 0) {
					m_currentSelectIndex = m_buttonPool->totalCount() - 1;
				}
			}

			if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_B) == BS_PRESSED) {
				m_xboxUsed = true;
				m_selected = true;
			}

			if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_START) == BS_PRESSED) {
				m_home = true;
			}

			if (m_xboxUsed) {
				if (GameObject* obj = m_buttonPool->getObjectAtIndex(m_currentSelectIndex)) {
					Button* button = static_cast<Button*>(obj);
					SetArrowPosXbox(button);
				}
			}
		}
	}
}

void DifficultyScene::Draw(Renderer& renderer)
{
	renderer.SetClearColour(0, 27, 30);

	m_backgroundPlanet->Draw(renderer);

	m_homeButton->Draw(renderer);

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

void DifficultyScene::DebugDraw()
{
}

char DifficultyScene::GetSelection()
{
	int index = 0;

	for (size_t i = 0; i < m_buttonPool->totalCount(); i++) {
		if (GameObject* obj = m_buttonPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<Button*>(obj)) {
				Button* button = static_cast<Button*>(obj);

				if (m_selectedButton.x == button->Position().x && m_selectedButton.y == button->Position().y) {
					index = i;
					break;
				}
			}
		}
	}

	switch (index) {
	case 0:
		m_selectedOption = 'E';
		break;
	case 1:
		m_selectedOption = 'N';
		break;
	case 2:
		m_selectedOption = 'H';
		break;
	}

	return m_selectedOption;
}

bool DifficultyScene::GetStatus()
{
	return m_sceneDone;
}

bool DifficultyScene::Home()
{
	return m_home;
}

bool DifficultyScene::IsColliding(const Box& box, GameObject* obj)
{
	if (!obj) return false;

	float bL, bR, bT, bB;

	if (Button* button = dynamic_cast<Button*>(obj)) {
		bL = button->Position().x - button->GetSpriteWidth() / 2;
		bR = bL + button->GetSpriteWidth();
		bT = button->Position().y - button->GetSpriteHeight() / 2;
		bB = bT + button->GetSpriteHeight();
	}
	else if (HomeButton* home = dynamic_cast<HomeButton*>(obj)) {
		bL = home->Position().x - home->GetSpriteWidth() / 2;
		bR = bL + home->GetSpriteWidth();
		bT = home->Position().y - home->GetSpriteHeight() / 2;
		bB = bT + home->GetSpriteHeight();
	}

	return !(box.x + box.width < bL ||
		box.x > bR ||
		box.y + box.height < bT ||
		box.y > bB);
}

bool DifficultyScene::CheckMousePos(InputSystem* inputSystem)
{
	Vector2 mousePos = inputSystem->GetMousePosition();
	bool mouseClicked = inputSystem->GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED;

	Box mouseBox(
		mousePos.x - 5.0f, mousePos.y - 5.0f,
		10.0f, 10.0f);

	auto potentialCollisions = m_collisionTree->queryRange(mouseBox);

	for (auto* obj : potentialCollisions) {
		if (Button* button = dynamic_cast<Button*>(obj)) {
			if (button && IsColliding(mouseBox, button)) {
				button->Pressed();
				for (size_t i = 0; i < m_arrowPool->totalCount(); i++) {
					if (GameObject* obj = m_arrowPool->getObjectAtIndex(i)) {
						if (obj && dynamic_cast<SelectionArrow*>(obj)) {
							SelectionArrow* arrow = static_cast<SelectionArrow*>(obj);
							arrow->Position() = button->Position();
							arrow->SetOffset((float)button->GetSpriteWidth());
							if (mouseClicked) {
								button->Pressed();
								arrow->Pressed();
								m_selectedButton = button->Position();
								m_sceneDone = true;
							}
						}
					}
				}
				break;
			}
		}
		else if (HomeButton* home = dynamic_cast<HomeButton*>(obj)) {
			if (home && IsColliding(mouseBox, home)) {
				home->Pressed();
				if (mouseClicked) {
					home->Pressed();
					m_home = true;
					break;
				}
			}
		}
	}

	return true;
}

void DifficultyScene::SetArrowPosXbox(Button* button)
{
	for (size_t i = 0; i < m_arrowPool->totalCount(); i++) {
		if (GameObject* obj = m_arrowPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<SelectionArrow*>(obj)) {
				SelectionArrow* arrow = static_cast<SelectionArrow*>(obj);
				arrow->Position() = button->Position();
				arrow->SetOffset((float)button->GetSpriteWidth());
				if (m_selected) {
					button->Pressed();
					arrow->Pressed();
					m_selectedButton = button->Position();
					m_sceneDone = true;
				}
			}
		}
	}
}
