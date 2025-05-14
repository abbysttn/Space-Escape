#ifndef _SHIPPART_H__
#define _SHIPPART_H__

#include "vector2.h"
#include "gameobject.h"
#include <string>

using namespace std;

class Renderer;
class Sprite;

class ShipPart : public GameObject {
public:
	ShipPart();
	~ShipPart();

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

	void Drop(Vector2 position);
	void Reset();

	void SetColour(float red, float green, float blue);
	int GetPartType();

	string GetFilepath();

protected:

private:
	ShipPart(const ShipPart& shipPart);
	ShipPart& operator=(const ShipPart& shipPart);

public:

protected:
	Vector2 m_position;
	Sprite* m_sprite;

	bool m_active;
	bool m_collected;

	Vector2 m_velocity;
	Vector2 m_startPos;
	float m_gravity;
	bool m_isDropping;
	int bounces;

	int m_shipPart;

private:
};

#endif // !_SHIPPART_H__
