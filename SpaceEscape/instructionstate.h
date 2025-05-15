#ifndef _INSTRUCTIONSTATE_H__
#define _INSTRUCTIONSTATE_H__

#include "gamestate.h"

class InstructionScene;

class InstructionState : public GameState {
public:
	InstructionState();

	void Enter() override;
	void Update(float deltatime) override;
	void Draw() override;
	void DebugDraw() override;
	void Exit() override;

	GameStates GetNextState() const override;

protected:

private:
	InstructionState(const InstructionState& difficultyMenuState);
	InstructionState& operator=(const InstructionState& difficultyMenuState);

public:

protected:
	InstructionScene* m_scene;
	GameStates m_nextState;

private:
};

#endif // !_INSTRUCTIONSTATE_H__
