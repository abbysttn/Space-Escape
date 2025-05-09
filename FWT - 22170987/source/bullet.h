#ifndef _BULLET_H__
#define _BULLET_H__

#include "vector2.h"
#include "gameobject.h"

using namespace std;

class Renderer;
class Sprite;

class Bullet : public GameObject {
public:
	Bullet();
	~Bullet();

	bool initialise(Renderer& renderer);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	Vector2& Position();

	int GetSpriteWidth();
	void SetColour(float red, float green, float blue);
	GameObject* Create() const override;
	bool isActive() const override;
	void reset() override;

	void SetActive(bool active);
	void Fire(Vector2& startPos, const Vector2& targetPos);

protected:

private:
	Bullet(const Bullet& bullet);
	Bullet& operator=(const Bullet& bullet);

public:

protected:
	Vector2 m_boundsLow;
	Vector2 m_boundsHigh;
	Vector2 m_position;
	Vector2 m_targetPos;
	Vector2 m_velocity;
	float m_speed;
	Sprite* m_sprite;

	bool m_alive;

private:

};

#endif // !_BULLET_H__
