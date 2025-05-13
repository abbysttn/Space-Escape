#ifndef _LIFE_H__
#define _LIFE_H__

#include "vector2.h"
#include "gameobject.h"

using namespace std;

class Renderer;
class Sprite;

class Life : public GameObject {
public:
	Life();
	~Life();

	bool initialise(Renderer& renderer);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	Vector2& Position();
	int GetSpriteWidth() const override;
	void SetColour(float red, float green, float blue);
	GameObject* Create() const override;
	bool isActive() const override;
	void reset() override;
	void SetActive(bool active);

protected:

private:
	Life(const Life& life);
	Life& operator=(const Life& life);

public:

protected:
	Vector2 m_position;
	Sprite* m_sprite;

	bool m_alive;

private:

};

#endif // !_LIFE_H__
