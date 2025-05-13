#ifndef _PLAYERSHIP_H__
#define _PLAYERSHIP_H__

#include "vector2.h"

using namespace std;

class Renderer;
class Sprite;

class PlayerShip {
public:
	PlayerShip();
	~PlayerShip();

	bool Initialise(Renderer& renderer);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	Vector2& Position();
	void SetRotation(float angle);
	void SetActive(bool active);

	void Fly(float deltaTime);
	void Fall(float deltaTime);

	bool IsActive();

protected:

private:
	PlayerShip(const PlayerShip& playerShip);
	PlayerShip& operator=(const PlayerShip& playerShip);

public:

protected:
	Vector2 m_position;
	Sprite* m_sprite;

	Vector2 m_boundsLow;
	Vector2 m_boundsHigh;

	bool m_active;


private:
};

#endif // !_PLAYERSHIP_H__
