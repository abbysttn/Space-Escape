#ifndef _STARTSCENE_H__
#define _STARTSCENE_H__

#include "scene.h"
#include <memory>

using namespace std;

class Renderer;
class InputSystem;
class Sprite;
class GameObjectPool;
class QuadTree;
struct Box;
class Button;

class StartScene : public Scene {
public:
	StartScene();
	virtual ~StartScene();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

protected:
	bool IsColliding(const Box& box, Button* button);
	bool CheckMousePos(InputSystem* inputSystem);

private:
	StartScene(const StartScene& startScene);
	StartScene& operator=(const StartScene& startScene);

public:

protected:
	GameObjectPool* m_textPool;
	GameObjectPool* m_buttonPool;
	GameObjectPool* m_arrowPool;
	Sprite* m_backgroundPlanet;

	unique_ptr<QuadTree> m_collisionTree;

private:
};

#endif // !_STARTSCENE_H__
