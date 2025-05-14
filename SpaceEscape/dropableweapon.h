#ifndef _DROPABLEWEAPON_H__
#define _DROPABLEWEAPON_H__

#include "vector2.h"
#include "gameobject.h"
#include <string>

using namespace std;

class Renderer;
class Sprite;

class DropableWeapon : public GameObject {
public:
	DropableWeapon();
	~DropableWeapon();

	bool Initialise(Renderer& renderer, int weapon);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	Vector2& Position();
	int GetSpriteWidth() const override;

	GameObject* Create() const override;
	bool isActive() const override;
	void reset() override;

	void SetActive(bool active);

	void SetCollected(bool collected);
	bool IsCollected();
	void Reset();

	void Drop(Vector2 position);

protected:
	string GetFilepath(int weapon);

private:
	DropableWeapon(const DropableWeapon& dropableWeapon);
	DropableWeapon& operator=(const DropableWeapon& dropableWeapon);

public:

protected:
	Vector2 m_position;
	Sprite* m_sprite;

	bool m_active;
	bool m_collected;

private:
};

#endif // !_DROPABLEWEAPON_H__