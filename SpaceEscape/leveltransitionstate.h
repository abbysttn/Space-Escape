#ifndef _LEVELTRANSITIONSTATE_H__
#define _LEVELTRANSITIONSTATE_H__

#include "gamestate.h"

class LevelTransition;

class LevelTransitionState : public GameState {
public:
	LevelTransitionState();

	void Enter() override;
	void Update(float deltatime) override;
	void Draw() override;
	void DebugDraw() override;
	void Exit() override;

	GameStates GetNextState() const override;

protected:

private:
	LevelTransitionState(const LevelTransitionState& levelTransitionState);
	LevelTransitionState& operator=(const LevelTransitionState& levelTransitionState);

public:

protected:
	LevelTransition* m_scene;
	GameStates m_nextState;

private:
};

#endif // !_LEVELTRANSITIONSTATE_H__
