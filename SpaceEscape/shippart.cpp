#include "shippart.h"

#include "Renderer.h"
#include "sprite.h"
#include "inlinehelpers.h"

#include <cassert>
#include <cstdlib>

ShipPart::ShipPart() : m_sprite(0), m_collected(false), m_gravity(500.0f), m_isDropping(false)
{
}

ShipPart::~ShipPart()
{
	delete m_sprite;
	m_sprite = 0;
}

bool ShipPart::Initialise(Renderer& renderer)
{
	string filepath = GetFilepath();
	m_sprite = renderer.CreateSprite(filepath.c_str());

	m_sprite->SetScale(3.0);

	m_position.x = 0;
	m_position.y = 0;
	return true;
}

void ShipPart::Process(float deltaTime)
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

void ShipPart::Draw(Renderer& renderer)
{
	m_sprite->Draw(renderer);
}

Vector2& ShipPart::Position()
{
	return m_position;
}

int ShipPart::GetSpriteWidth() const
{
	return m_sprite->GetWidth();
}

GameObject* ShipPart::Create() const
{
	return new ShipPart();
}

bool ShipPart::isActive() const
{
	return m_active;
}

void ShipPart::reset()
{
	m_position.x = 0;
	m_position.y = 0;
}

void ShipPart::SetActive(bool active)
{
	m_active = active;
}

void ShipPart::SetRotation(float angle)
{
	m_sprite->SetAngle(angle);
}

void ShipPart::SetCollected(bool collected)
{
	m_collected = collected;
}

bool ShipPart::IsCollected()
{
	return m_collected;
}

void ShipPart::Drop(Vector2 position)
{
	m_position = position;
	m_startPos = position;
	bounces = 2;
	m_velocity = { 0, -100.0f };
	m_isDropping = true;
	m_active = true;
	m_collected = false;
}

void ShipPart::Reset()
{
	m_active = false;
	m_collected = false;
}

void ShipPart::SetColour(float red, float green, float blue)
{
	m_sprite->SetRedTint(red);
	m_sprite->SetGreenTint(green);
	m_sprite->SetBlueTint(blue);
}

int ShipPart::GetPartType()
{
	return m_shipPart;
}

string ShipPart::GetFilepath()
{
	int randomPart = GetRandom(1, 4);
	m_shipPart = randomPart;

	string filepath = "..\\assets\\";

	switch (randomPart) {
	case 1:
		filepath += "copper.png";
		break;

	case 2:
		filepath += "metal_sheet.png";
		break;

	case 3:
		filepath += "wood_plank.png";
		break;

	case 4:
		filepath += "metal_pieces.png";
		break;

	default:
		filepath += "copper.png";
		break;
	}

	return filepath;
}
