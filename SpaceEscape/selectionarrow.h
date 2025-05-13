#ifndef _SELECTIONARROW_H__
#define _SELECTIONARROW_H__

#include "vector2.h"
#include "gameobject.h"

using namespace std;

class Renderer;
class AnimatedSprite;

class SelectionArrow : public GameObject {
public:
	SelectionArrow();
	~SelectionArrow();

	bool initialise(Renderer& renderer, char direction);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	Vector2& Position();
	int GetSpriteWidth() const override;
	int GetSpriteHeight();
	GameObject* Create() const override;
	bool isActive() const override;
	void reset() override;
	void SetOffset(float offset);

	void Pressed();

protected:
	void ResetButtonState();

private:
	SelectionArrow(const SelectionArrow& selectionArrow);
	SelectionArrow& operator=(const SelectionArrow& selectionArrow);

public:

protected:
	Vector2 m_position;
	AnimatedSprite* m_sprite;

	char m_direction;

	float m_offset;

private:

};

#endif // !_SELECTIONARROW_H__
