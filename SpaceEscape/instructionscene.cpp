#include "instructionscene.h"

#include "Renderer.h"
#include "inputsystem.h"
#include "xboxcontroller.h"

#include "textrenderer.h"
#include "button.h"
#include "selectionarrow.h"
#include "gameobjectpool.h"
#include "instruction.h"

#include "homebutton.h"

#include "logmanager.h"

#include "quadtree.h"

InstructionScene::InstructionScene() : m_arrowPool(nullptr), m_title(0), m_home(false), m_currentInstruction(0), m_instructionsPool(nullptr),
m_homeButton(0) {}

InstructionScene::~InstructionScene()
{
	delete m_arrowPool;
	delete m_title;
	delete m_instructionsPool;
	delete m_homeButton;
}

bool InstructionScene::Initialise(Renderer& renderer)
{
	screenWidth = renderer.GetWidth();
	screenHeight = renderer.GetHeight();

	m_arrowPool = new GameObjectPool(SelectionArrow(), 2);

	m_title = new TextRenderer();
	m_title->initialise(renderer, "How To Play", 150);
	m_title->Position() = { screenWidth / 2.0f, (screenHeight / 2.0f) - 300.0f };

	m_homeButton = new HomeButton();
	m_homeButton->initialise(renderer);
	m_homeButton->Position() = { screenWidth - 48.0f, screenHeight - 48.0f };
	
	m_instructionsPool = new GameObjectPool(Instruction(), 5);

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

	for (size_t i = 0; i < m_instructionsPool->totalCount(); i++) {
		if (GameObject* obj = m_instructionsPool->getObjectAtIndex(i)) {
			Instruction* file = static_cast<Instruction*>(obj);
			switch (i) {
			case 0:
				file->initialise(renderer, "move");
				break;
			case 1:
				file->initialise(renderer, "attack");
				break;
			case 2:
				file->initialise(renderer, "obtain");
				break;
			case 3:
				file->initialise(renderer, "pause");
				break;
			case 4:
				file->initialise(renderer, "menu");
				break;
			}
		}
	}

	m_collisionTree = make_unique<QuadTree>(Box(0.0f, 0.0f, (float)renderer.GetWidth(), (float)renderer.GetHeight()));

	return true;
}

void InstructionScene::Process(float deltaTime, InputSystem& inputSystem)
{
	m_collisionTree->clear();

	m_title->Process(deltaTime);

	m_homeButton->Process(deltaTime);
	Box homeRange(
		m_homeButton->Position().x - m_homeButton->GetSpriteWidth() / 2,
		m_homeButton->Position().y - m_homeButton->GetSpriteHeight() / 2,
		(float)m_homeButton->GetSpriteWidth(),
		(float)m_homeButton->GetSpriteHeight()
	);

	m_collisionTree->insert(m_homeButton, homeRange);

	if (GameObject* obj = m_instructionsPool->getObjectAtIndex(m_currentInstruction)) {
		if (obj && dynamic_cast<Instruction*>(obj)) {
			Instruction* file = static_cast<Instruction*>(obj);
			file->Position() = { screenWidth / 2.0f, (screenHeight / 2.0f) + 50.0f };
			file->Process(deltaTime);

			m_arrowPos = file->Position();
			m_arrowOffset = file->GetSpriteWidth() / 1.5f;
		}
	}

	for (size_t i = 0; i < m_arrowPool->totalCount(); i++) {
		if (GameObject* obj = m_arrowPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<SelectionArrow*>(obj)) {
				SelectionArrow* arrow = static_cast<SelectionArrow*>(obj);
				arrow->Process(deltaTime);
				arrow->Position() = m_arrowPos;
				if (i == 1) {
					arrow->Position().x += m_arrowOffset;
				}
				else {
					arrow->Position().x -= m_arrowOffset;
				}

				Box arrowRange(
					arrow->Position().x - arrow->GetSpriteWidth() / 2,
					arrow->Position().y - arrow->GetSpriteHeight() / 2,
					(float)arrow->GetSpriteWidth(),
					(float)arrow->GetSpriteHeight()
				);

				m_collisionTree->insert(arrow, arrowRange);
			}
		}
	}

	if (inputSystem.GetNumberOfControllersAttached() > 0) {

		XboxController* controller = new XboxController();
		controller = inputSystem.GetController(0);

		if (controller) {
			if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_LEFT) == BS_PRESSED) {
				m_currentInstruction--;
				if (m_currentInstruction < 0) {
					m_currentInstruction = 4;
				}
			}
			else if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == BS_PRESSED) {
				m_currentInstruction++;
				if (m_currentInstruction >= 5) {
					m_currentInstruction = 0;
				}
			}

			if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_START) == BS_PRESSED) {
				m_home = true;
			}
		}
	}

	bool clicked = CheckMousePos(&inputSystem);
}

void InstructionScene::Draw(Renderer& renderer)
{
	renderer.SetClearColour(71, 171, 169);

	m_title->Draw(renderer);

	m_homeButton->Draw(renderer);
	
	for (size_t i = 0; i < m_arrowPool->totalCount(); i++) {
		if (GameObject* obj = m_arrowPool->getObjectAtIndex(i)) {
			if (obj && obj->isActive()) {
				SelectionArrow* arrow = static_cast<SelectionArrow*>(obj);
				arrow->Draw(renderer);
			}
		}
	}

	if (GameObject* obj = m_instructionsPool->getObjectAtIndex(m_currentInstruction)) {
		if (obj && obj->isActive()) {
			Instruction* file = static_cast<Instruction*>(obj);
			file->Draw(renderer);
		}
	}
}

void InstructionScene::DebugDraw()
{
}

void InstructionScene::GetSelection()
{
	const float tolerance = 5.0f;

	int index = -1;

	for (size_t i = 0; i < m_arrowPool->totalCount(); i++) {
		if (GameObject* obj = m_arrowPool->getObjectAtIndex(i)) {
			if (obj && dynamic_cast<SelectionArrow*>(obj)) {
				SelectionArrow* arrow = static_cast<SelectionArrow*>(obj);
				if (m_selectedButton.x == arrow->Position().x && m_selectedButton.y == arrow->Position().y) {
					index = i;
					break;
				}
			}
		}
	}

	switch (index) {
	case 0:
		m_currentInstruction--;
		if (m_currentInstruction < 0) {
			m_currentInstruction = 4;
		}
		break;
	case 1:
		m_currentInstruction++;
		if (m_currentInstruction >= 5) {
			m_currentInstruction = 0;
		}
		break;
	case 2:
		m_home = true;
		break;
	default:
		LogManager::GetInstance().Log("Not valid click found!");
		break;
	}
}

bool InstructionScene::Home()
{
	return m_home;
}

bool InstructionScene::IsColliding(const Box& box, GameObject* obj)
{
	if (!obj) return false;

	float bL, bR, bT, bB;

	if (SelectionArrow* button = dynamic_cast<SelectionArrow*>(obj)) {
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

bool InstructionScene::CheckMousePos(InputSystem* inputSystem)
{
	Vector2 mousePos = inputSystem->GetMousePosition();
	bool mouseClicked = inputSystem->GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED;

	Box mouseBox(
		mousePos.x - 5.0f, mousePos.y - 5.0f,
		10.0f, 10.0f);

	auto potentialCollisions = m_collisionTree->queryRange(mouseBox);

	for (auto* obj : potentialCollisions) {
		if (SelectionArrow* button = dynamic_cast<SelectionArrow*>(obj)) {
			if (button && IsColliding(mouseBox, button)) {
				button->Pressed();
				if (mouseClicked) {
					button->Pressed();
					m_selectedButton = button->Position();
					GetSelection();
					break;
				}
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