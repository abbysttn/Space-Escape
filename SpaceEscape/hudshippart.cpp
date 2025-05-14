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

bool HUDShipPart::Initialise(Renderer& renderer, int part)
{
	string filepath = GetFilepath(part);

	m_sprite = renderer.CreateSprite(filepath.c_str());

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

int HUDShipPart::GetSpriteWidth() const
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

string HUDShipPart::GetFilepath(int part)
{
	string filepath = "..\\assets\\";

	switch (part) {
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
