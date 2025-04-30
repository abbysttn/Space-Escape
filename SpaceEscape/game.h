#ifndef __GAME_H
#define __GAME_H

#include <vector>

#define DEBUG 1
#define USE_LAG 0


using namespace std;

class Renderer;

class AnimatedSprite;

class Sprite;

class Scene;

class InputSystem;

class INIParser;

class Game {
	//member methods:
public:
	static Game& GetInstance();
	static void DestroyInstance();

	bool Initialise();
	bool DoGameLoop();
	void Quit();
	void ToggleDebugWindow();

protected:
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	void DebugDraw();

	void ProcessFrameCounting(float deltaTime);

private:
	Game();
	~Game();
	Game(const Game& game);
	Game& operator=(const Game& game);

	//Member data:

public:

protected:
	static Game* sm_pInstance;
	Renderer* m_pRenderer;
	InputSystem* m_pInputSystem;

	__int64 m_iLastTime;
	float m_fExecutionTime;
	float m_fElapsedSeconds;
	int m_iFrameCount;
	int m_iFPS;

#if DEBUG
	bool m_bShowDebugWindow = true;
	bool m_bPauseSimulation = false;
#endif // DEBUG

	vector<Scene*> m_scenes;
	int m_iCurrentScene;

#if USE_LAG
	float m_fLag;
	int m_iUpdateCount;
#endif // USE_LAG

	bool m_bLooping;

private:
};

#endif //__GAME_H