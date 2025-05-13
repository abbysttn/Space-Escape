#include "hudshippart.h"

#include "Renderer.h"
#include "sprite.h"

#include <cassert>
#include <cstdlib>

HUDShipPart::HUDShipPart() : m_sprite(0), m_collected(false) {}

HUDShipPart::~HUDShipPart()
{
	delete m_sprite;
	m_sprite = 0;
}

bool HUDShipPart::Initialise(Renderer& renderer)
{
	m_sprite = renderer.CreateSprite("..\\assets\\pot.png");

	m_sprite->SetScale(3.0);

	m_position.x = 0;
	m_position.y = 0;
	return true;
}

void HUDShipPart::Process(float deltaTime)
{
	if (m_active) {
		m_sprite->SetX(static_cast<int>(m_position.x));
		m_sprite->SetY(static_cast<int>(m_position.y));
	}
	m_sprite->Process(deltaTime);
}

void HUDShipPart::Draw(Renderer& renderer)
{
	m_sprite->Draw(renderer);
}

Vector2& HUDShipPart::Position()
{
	return m_position;
}

int HUDShipPart::GetSpriteWidth()
{
	return m_sprite->GetWidth();
}

GameObject* HUDShipPart::Create() const
{
	return new HUDShipPart();
}

bool HUDShipPart::isActive() const
{
	return m_active;
}

void HUDShipPart::reset()
{
	m_position.x = 0;
	m_position.y = 0;
}

void HUDShipPart::SetActive(bool active)
{
	m_active = active;
}

void HUDShipPart::SetRotation(float angle)
{
	m_sprite->SetAngle(angle);
}

void HUDShipPart::SetCollected(bool collected)
{
	m_collected = collected;
}

bool HUDShipPart::IsCollected()
{
	return m_collected;
}

void HUDShipPart::Reset()
{
	m_active = false;
	m_collected = false;
}

void HUDShipPart::SetColour(float red, float green, float blue)
{
	m_sprite->SetRedTint(red);
	m_sprite->SetGreenTint(green);
	m_sprite->SetBlueTint(blue);
}
