#ifndef _CENTER_H__
#define _CENTER_H__

#include "vector2.h"
#include "gameobject.h"

using namespace std;

class Renderer;
class Sprite;

class Center : public GameObject {
public:
	Center();
	~Center();

	bool initialise(Renderer& renderer, const char* filename);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	Vector2& Position();
	int GetSpriteWidth() const override;
	void SetColour(float red, float green, float blue);
	GameObject* Create() const override;
	bool isActive() const override;
	void reset() override;
	void SetActive(bool active);
	void SetRotation(float angle);

protected:

private:
	Center(const Center& center);
	Center& operator=(const Center& center);

public:

protected:
	Vector2 m_position;
	Sprite* m_sprite;

	bool m_alive;

private:

};

#endif // !_CENTER_H__
