#ifndef _HOMEBUTTON_H__
#define _HOMEBUTTON_H__

#include "vector2.h"
#include "gameobject.h"

using namespace std;

class Renderer;
class AnimatedSprite;

class HomeButton : public GameObject {
public:
	HomeButton();
	~HomeButton();

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
	HomeButton(const HomeButton& homeButton);
	HomeButton& operator=(const HomeButton& homeButton);

public:

protected:
	Vector2 m_position;
	AnimatedSprite* m_sprite;

private:

};

#endif // !_HOMEBUTTON_H__
