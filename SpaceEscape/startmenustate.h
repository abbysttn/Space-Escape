#ifndef _STARTMENUSTATE_H__
#define _STARTMENUSTATE_H__

#include "gamestate.h"

class StartScene;

class StartMenuState : public GameState {
public:
	StartMenuState();

	void Enter() override;
	void Update(float deltatime) override;
	void Draw() override;
	void DebugDraw() override;
	void Exit() override;

	GameStates GetNextState() const override;

protected:

private:
	StartMenuState(const StartMenuState& startMenuState);
	StartMenuState& operator=(const StartMenuState& startMenuState);

public:

protected:
	StartScene* m_scene;
	GameStates m_nextState;

private:
};

#endif // !_STARTMENUSTATE_H__
