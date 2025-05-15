#ifndef _PAUSESCENE_H__
#define _PAUSESCENE_H__

#include "scene.h"
#include "vector2.h"
#include <memory>

using namespace std;

class Renderer;
class InputSystem;
class Sprite;
class GameObject;
class GameObjectPool;
class QuadTree;
struct Box;
class Button;
class HomeButton;

class PauseScene : public Scene {
public:
	PauseScene();
	virtual ~PauseScene();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

	char GetSelection();
	bool Quit();
	bool Resume();
	bool Home();
	void Paused();

protected:
	bool IsColliding(const Box& box, GameObject* obj);
	bool CheckMousePos(InputSystem* inputSystem);
	void SetArrowPosXbox(Button* button);

private:
	PauseScene(const PauseScene& pauseScene);
	PauseScene& operator=(const PauseScene& pauseScene);

public:

protected:
	HomeButton* m_homeButton;
	GameObjectPool* m_textPool;
	GameObjectPool* m_buttonPool;
	GameObjectPool* m_arrowPool;
	Sprite* m_background;

	unique_ptr<QuadTree> m_collisionTree;

	bool m_quit;
	bool m_resume;

	Vector2 m_selectedButton;
	char m_selectedOption;

	int m_currentSelectIndex;

	bool m_selected;
	bool m_xboxUsed;

	bool m_home;

private:
};

#endif // !_PAUSESCENE_H__
