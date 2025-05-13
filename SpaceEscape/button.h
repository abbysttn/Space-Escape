#ifndef _BUTTON_H__
#define _BUTTON_H__

#include "vector2.h"
#include "gameobject.h"

using namespace std;

class Renderer;
class AnimatedSprite;

class Button : public GameObject {
public:
	Button();
	~Button();

	bool initialise(Renderer& renderer);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	Vector2& Position();
	int GetSpriteWidth() const override;
	int GetSpriteHeight();
	GameObject* Create() const override;
	bool isActive() const override;
	void reset() override;
	
	void Pressed();

protected:
	void ResetButtonState();

private:
	Button(const Button& button);
	Button& operator=(const Button& button);

public:

protected:
	Vector2 m_position;
	AnimatedSprite* m_sprite;

private:

};

#endif // !_BUTTON_H__
