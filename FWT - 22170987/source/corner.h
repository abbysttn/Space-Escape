#ifndef _CORNER_H__
#define _CORNER_H__

#include "vector2.h"
#include "gameobject.h"

using namespace std;

class Renderer;
class Sprite;

class Corner : public GameObject {
public:
	Corner();
	~Corner();

	bool initialise(Renderer& renderer, const char* filename);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	Vector2& Position();
	int GetSpriteWidth();
	void SetColour(float red, float green, float blue);
	GameObject* Create() const override;
	bool isActive() const override;
	void reset() override;
	void SetActive(bool active);
	void SetRotation(float angle);

protected:

private:
	Corner(const Corner& corner);
	Corner& operator=(const Corner& corner);

public:

protected:
	Vector2 m_position;
	Sprite* m_sprite;

	bool m_alive;

private:
	
};

#endif // !_CORNER_H__
