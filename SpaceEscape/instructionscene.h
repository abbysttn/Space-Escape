#ifndef _INSTRUCTIONSCENE_H__
#define _INSTRUCTIONSCENE_H__

#include "scene.h"
#include "vector2.h"
#include <memory>

using namespace std;

class Renderer;
class InputSystem;
class GameObjectPool;
class GameObject;
class TextRenderer;
class QuadTree;
struct Box;
class Button;
class SelectionArrow;
class Instruction;
class TileParser;
class Player;
class HomeButton;

class InstructionScene : public Scene {
public:
	InstructionScene();
	virtual ~InstructionScene();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

	bool Home();

protected:
	bool IsColliding(const Box& box, GameObject* obj);
	bool CheckMousePos(InputSystem* inputSystem);
	void GetSelection();

private:
	InstructionScene(const InstructionScene& instructionScene);
	InstructionScene& operator=(const InstructionScene& instructionScene);

public:

protected:
	HomeButton* m_homeButton;
	GameObjectPool* m_arrowPool;
	GameObjectPool* m_instructionsPool;
	TextRenderer* m_title;

	unique_ptr<QuadTree> m_collisionTree;

	bool m_home;

	Vector2 m_arrowPos;
	float m_arrowOffset;

	Vector2 m_selectedButton;
	char m_selectedOption;

	int m_currentInstruction;

	int screenWidth;
	int screenHeight;

	bool m_selected;
	bool m_xboxUsed;

private:
};

#endif // !_INSTRUCTIONSCENE_H__
