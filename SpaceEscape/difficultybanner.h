#ifndef _DIFFICULTYBANNER_H__
#define _DIFFICULTYBANNER_H__

#include "scene.h"
#include <string>

using namespace std;

class TextRenderer;
class Sprite;
class Renderer;
class InputSystem;

class DifficultyBanner : public Scene {
public:
	DifficultyBanner(int effect, char diff, int level);
	virtual ~DifficultyBanner();
	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

	bool GetStatus();

protected:
	string GetPlanetEffect(int& planetEffectNumber);
	string GetDifficulty(char& difficultyChar);

private:
	DifficultyBanner(const DifficultyBanner& difficultyBanner);
	DifficultyBanner& operator=(const DifficultyBanner& difficultyBanner);

public:

protected:
	TextRenderer* m_difficulty;
	TextRenderer* m_planetEffect;

	float screenOffsetX;
	float screenOffsetY;

	Sprite* m_banner;

	string difficulty;
	char difficultyChar;
	string planetEffect;
	int planetEffectNumber;

	bool m_sceneDone;

	float m_timer;
	float m_maxTime;

	int m_levelNumber;

private:
};

#endif // !_DIFFICULTYBANNER_H__
