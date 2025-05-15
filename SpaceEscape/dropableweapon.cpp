#include "dropableweapon.h"

#include "Renderer.h"
#include "sprite.h"

#include <cassert>
#include <cstdlib>

DropableWeapon::DropableWeapon() : m_sprite(0), m_collected(false), m_gravity(500.0f), m_isDropping(false), m_active(false) {}

DropableWeapon::~DropableWeapon()
{
	delete m_sprite;
	m_sprite = 0;
}

bool DropableWeapon::Initialise(Renderer& renderer, int weapon)
{
	string filepath = GetFilepath(weapon);
	m_sprite = renderer.CreateSprite(filepath.c_str());

	m_sprite->SetScale(1.0);

	m_position.x = 0;
	m_position.y = 0;
	return true;
}

void DropableWeapon::Process(float deltaTime)
{
	if (m_active && !m_collected) {

		if (m_isDropping) {
			m_velocity.y += m_gravity * deltaTime;

			m_position += m_velocity * deltaTime;

			float ground = m_startPos.y + 30.0f;

			if (m_position.y >= ground && m_velocity.y > 0) {
				if (bounces > 0) {
					m_position.y = ground;
					m_velocity.y = -m_velocity.y * 0.6f;
					m_velocity.x *= 0.8f;
					bounces--;
				}
				else {
					m_position.y = ground;
					m_velocity = { 0, 0 };
					m_isDropping = false;
				}
			}
		}

		m_sprite->SetX(static_cast<int>(m_position.x));
		m_sprite->SetY(static_cast<int>(m_position.y));
	}
	m_sprite->Process(deltaTime);
}

void DropableWeapon::Draw(Renderer& renderer)
{
	m_sprite->Draw(renderer);
}

Vector2& DropableWeapon::Position()
{
	return m_position;
}

int DropableWeapon::GetSpriteWidth() const
{
	return m_sprite->GetWidth();
}

GameObject* DropableWeapon::Create() const
{
	return new DropableWeapon();
}

bool DropableWeapon::isActive() const
{
	return m_active;
}

void DropableWeapon::reset()
{
	m_position.x = 0;
	m_position.y = 0;
}

void DropableWeapon::SetActive(bool active)
{
	m_active = active;
}

void DropableWeapon::SetCollected(bool collected)
{
	m_collected = collected;
}

bool DropableWeapon::IsCollected()
{
	return m_collected;
}

void DropableWeapon::Reset()
{
	m_active = false;
	m_collected = false;
}

void DropableWeapon::Drop(Vector2 position)
{
	m_position = position;
	m_startPos = position;
	bounces = 2;
	m_velocity = { 0, -100.0f };
	m_isDropping = true;
	m_active = true;
	m_collected = false;
}

string DropableWeapon::GetFilepath(int weapon)
{
	string filepath = "..\\assets\\";

	switch (weapon) {
	case 0:
		filepath += "basic_knife.png";
		break;

	case 1:
		filepath += "upgraded_knife.png";
		break;

	case 2:
		filepath += "basic_gun.png";
		break;

	case 3:
		filepath += "upgraded_gun.png";
		break;

	default:
		filepath += "basic_knife.png";
		break;
	}

	return filepath;
}
