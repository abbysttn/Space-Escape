#ifndef _WATER_H__
#define _WATER_H__

#include "vector2.h"
#include "gameobject.h"

using namespace std;

class Renderer;
class Sprite;

class Water : public GameObject {
public:
	Water();
	~Water();

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

protected:

private:
	Water(const Water& water);
	Water& operator=(const Water& water);

public:

protected:
	Vector2 m_position;
	Sprite* m_sprite;

	bool m_alive;

private:

};

#endif // !_WATER_H__
