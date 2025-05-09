#ifndef _STARTSCENE_H__
#define _STARTSCENE_H__

#include "scene.h"

using namespace std;

class Renderer;
class InputSystem;
class Sprite;

class StartScene : public Scene {
public:
	StartScene();
	virtual ~StartScene();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

protected:

private:
	StartScene(const StartScene& startScene);
	StartScene& operator=(const StartScene& startScene);

public:

protected:
	Sprite* m_pText;
	Sprite* m_button;

private:
};

#endif // !_STARTSCENE_H__
