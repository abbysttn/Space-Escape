#ifndef _HUDSHIPPART_H__
#define _HUDSHIPPART_H__

#include "vector2.h"
#include "gameobject.h"

using namespace std;

class Renderer;
class Sprite;

class HUDShipPart : public GameObject {
public:
	HUDShipPart();
	~HUDShipPart();

	bool Initialise(Renderer& renderer);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	Vector2& Position();
	int GetSpriteWidth() const override;

	GameObject* Create() const override;
	bool isActive() const override;
	void reset() override;

	void SetActive(bool active);

	void SetRotation(float angle);
	void SetCollected(bool collected);
	bool IsCollected();
	void Reset();

	void SetColour(float red, float green, float blue);

protected:

private:
	HUDShipPart(const HUDShipPart& shipPart);
	HUDShipPart& operator=(const HUDShipPart& shipPart);

public:

protected:
	Vector2 m_position;
	Sprite* m_sprite;

	bool m_active;
	bool m_collected;

private:
};

#endif // !_HUDSHIPPART_H__