#ifndef _EDGECORNER_H__
#define _EDGECORNER_H__

#include "vector2.h"
#include "gameobject.h"

using namespace std;

class Renderer;
class Sprite;

class EdgeCorner : public GameObject {
public:
	EdgeCorner();
	~EdgeCorner();

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
	EdgeCorner(const EdgeCorner& edgeCorner);
	EdgeCorner& operator=(const EdgeCorner& edgeCorner);

public:

protected:
	Vector2 m_position;
	Sprite* m_sprite;

	bool m_alive;

private:

};

#endif // !_EDGECORNER_H__
