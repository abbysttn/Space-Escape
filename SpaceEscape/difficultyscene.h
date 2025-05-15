#ifndef _DIFFICULTYSCENE_H__
#define _DIFFICULTYSCENE_H__

#include "scene.h"
#include "vector2.h"
#include <memory>

using namespace std;

class Renderer;
class InputSystem;
class Sprite;
class GameObjectPool;
class GameObject;
class QuadTree;
struct Box;
class Button;
class HomeButton;

class DifficultyScene : public Scene {
public:
	DifficultyScene();
	virtual ~DifficultyScene();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

	char GetSelection();
	bool GetStatus();
	bool Home();

protected:
	bool IsColliding(const Box& box, GameObject* obj);
	bool CheckMousePos(InputSystem* inputSystem);
	void SetArrowPosXbox(Button* button);

private:
	DifficultyScene(const DifficultyScene& difficultyScene);
	DifficultyScene& operator=(const DifficultyScene& difficultyScene);

public:

protected:
	HomeButton* m_homeButton;
	GameObjectPool* m_textPool;
	GameObjectPool* m_buttonPool;
	GameObjectPool* m_arrowPool;
	Sprite* m_backgroundPlanet;

	unique_ptr<QuadTree> m_collisionTree;

	bool m_sceneDone;

	Vector2 m_selectedButton;
	char m_selectedOption;

	int m_currentSelectIndex;

	bool m_selected;

	bool m_xboxUsed;
	bool m_home;

private:
};

#endif // !_DIFFICULTYSCENE_H__
