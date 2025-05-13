#ifndef _GAMEOVERSTATE_H__
#define _GAMEOVERSTATE_H__

#include "gamestate.h"

class GameOverScene;

class  GameOverState : public GameState {
public:
	GameOverState();

	void Enter() override;
	void Update(float deltatime) override;
	void Draw() override;
	void DebugDraw() override;
	void Exit() override;

	GameStates GetNextState() const override;

protected:

private:
	GameOverState(const GameOverState& gameOverState);
	GameOverState& operator=(const GameOverState& gameOverState);

public:

protected:
	GameOverScene* m_scene;
	GameStates m_nextState;

private:
};

#endif // !_GAMEOVERSTATE_H__
