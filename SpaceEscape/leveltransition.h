#ifndef _LEVELTRANSITION_H__
#define _LEVELTRANSITION_H__

#include "scene.h"
#include "vector"
#include "vector2.h"
#include <memory>

using namespace std;

class Renderer;
class InputSystem;
class Player;
class Sprite;

class LevelTransition : public Scene {
public:
	LevelTransition();
	virtual ~LevelTransition();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

	bool GetStatus();

protected:
	void GenerateFibonacciSpiral(int points, float centerX, float centerY);
	void AnimatePlayerSwirl();
	float Linear(float a, float b, float t);

private:
	LevelTransition(const LevelTransition& startScene);
	LevelTransition& operator=(const LevelTransition& startScene);

public:

protected:
	Player* m_player;
	Sprite* m_blackHole;

	bool m_sceneDone;

	vector<Vector2> m_fibonacciPoints;
	float m_swirlProgress;
	float m_swirlTime;
	Vector2 m_startPos;

private:
};

#endif // !_LEVELTRANSITION_H__
