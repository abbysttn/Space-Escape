#ifndef _INSTRUCTION_H__
#define _INSTRUCTION_H__

#include "vector2.h"
#include "gameobject.h"
#include <string>

using namespace std;

class Renderer;
class Sprite;

class Instruction : public GameObject {
public:
	Instruction();
	~Instruction();

	bool initialise(Renderer& renderer, string type);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	Vector2& Position();

	int GetSpriteWidth() const override;
	GameObject* Create() const override;
	bool isActive() const override;
	void reset() override;

protected:

private:
	Instruction(const Instruction& instruction);
	Instruction& operator=(const Instruction& instruction);

public:

protected:
	Vector2 m_position;
	Sprite* m_sprite;

	bool m_alive;

private:

};

#endif // !_INSTRUCTION_H__
