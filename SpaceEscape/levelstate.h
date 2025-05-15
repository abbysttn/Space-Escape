#ifndef _LEVELSTATE_H__
#define _LEVELSTATE_H__

#include "gamestate.h"
#include <string>

using namespace std;

class Level;

class LevelState : public GameState {
public:
	LevelState();

	void Enter() override;
	void Update(float deltatime) override;
	void Draw() override;
	void DebugDraw() override;
	void Exit() override;

	GameStates GetNextState() const override;

protected:
	char GetRandomLevelDifficulty();
	char GetDifficulty();
	int GetRandomLevelMap();
	string GetRandomLevelType();
	int GetRandomPlanetEffect();

private:
	LevelState(const LevelState& levelState);
	LevelState& operator=(const LevelState& levelState);

public:

protected:
	Level* m_scene;
	GameStates m_nextState;

	int m_levelNumber = 1;

private:
};

#endif // !_LEVELSTATE_H__
