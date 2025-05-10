#ifndef _SPLASHAUTSTATE_H__
#define _SPLASHAUTSTATE_H__

#include "gamestate.h"

class SceneSplashFMOD;

class SplashAUTState : public GameState {
public:
	void Enter() override;
	void Update() override;
	void Exit() override;

	GameStates GetNextState() const override;

protected:

private:

public:

protected:
	SceneSplashFMOD* m_scene;
	GameStates m_nextState;

private:
};

#endif // !_SPLASHAUTSTATE_H__
