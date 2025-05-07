#ifndef _WEAPON_H__
#define _WEAPON_H__

#include "vector2.h"
#include "gameobject.h"
#include <string>

using namespace std;

class Renderer;
class Sprite;

class Weapon : public GameObject {
public:
	Weapon();
	~Weapon();

	bool initialise(Renderer& renderer, const char* filePath);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	Vector2& Position();


	int GetSpriteWidth();
	int GetSpriteHeight();
	void SetColour(float red, float green, float blue);


	GameObject* Create() const override;
	bool isActive() const override;
	void reset() override;


	void SetRotation(float angle);
	void SetWeapon(bool isWeapon);
	void Swing();
	void SetWeaponType(char type);
	char GetWeaponType();

	void SetSize(float size);
	void SetOffset(float offset);
	float GetOffset();
	bool IsSwinging();

protected:

private:
	Weapon(const Weapon& weapon);
	Weapon& operator=(const Weapon& weapon);

public:

protected:
	Vector2 m_position;
	Sprite* m_sprite;

	bool m_isWeapon;
	bool m_isSwinging;
	bool m_isLeft;

	char m_weaponType;

	float m_swingSpeed;
	float m_swingAngle;
	float m_swingDirection;
	float m_maxRightSwingAngle;
	float m_maxLeftSwingAngle;
	float m_returnSpeed;

	float weaponOffset;

private:

};

#endif // !_WEAPON_H__
