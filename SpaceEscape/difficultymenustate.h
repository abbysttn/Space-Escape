#ifndef _DIFFICULTYMENUSTATE_H__
#define _DIFFICULTYMENUSTATE_H__

#include "gamestate.h"

class DifficultyScene;

class DifficultyMenuState : public GameState {
public:
	DifficultyMenuState();

	void Enter() override;
	void Update(float deltatime) override;
	void Draw() override;
	void DebugDraw() override;
	void Exit() override;

	GameStates GetNextState() const override;

protected:

private:
	DifficultyMenuState(const DifficultyMenuState& difficultyMenuState);
	DifficultyMenuState& operator=(const DifficultyMenuState& difficultyMenuState);

public:

protected:
	DifficultyScene* m_scene;
	GameStates m_nextState;

private:
};

#endif // !_DIFFICULTYMENUSTATE_H__
