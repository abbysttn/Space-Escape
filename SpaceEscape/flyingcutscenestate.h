#ifndef _FLYINGCUTSCENESTATE_H__
#define _FLYINGCUTSCENESTATE_H__

#include "gamestate.h"

class FlyingCutscene;

class FlyingCutsceneState : public GameState {
public:
	FlyingCutsceneState();

	void Enter() override;
	void Update(float deltatime) override;
	void Draw() override;
	void DebugDraw() override;
	void Exit() override;

	GameStates GetNextState() const override;

protected:

private:
	FlyingCutsceneState(const FlyingCutsceneState& startMenuState);
	FlyingCutsceneState& operator=(const FlyingCutsceneState& startMenuState);

public:

protected:
	FlyingCutscene* m_scene;
	GameStates m_nextState;

private:
};

#endif // !_FLYINGCUTSCENESTATE_H__
