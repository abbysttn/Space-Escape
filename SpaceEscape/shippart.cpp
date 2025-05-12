#include "shippart.h"

#include "Renderer.h"
#include "sprite.h"

#include "itemmanager.h"

#include <cassert>
#include <cstdlib>

ShipPart::ShipPart() : m_sprite(0), m_collected(false) 
{
	ItemManager::GetInstance().RegisterItem(this);
}

ShipPart::~ShipPart()
{
	ItemManager::GetInstance().UnregisterItem(this);

	delete m_sprite;
	m_sprite = 0;
}

bool ShipPart::Initialise(Renderer& renderer)
{
	m_sprite = renderer.CreateSprite("");

	m_sprite->SetScale(3.0);

	m_position.x = 0;
	m_position.y = 0;
	return true;
}

void ShipPart::Process(float deltaTime)
{
	if (m_active) {
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

int ShipPart::GetSpriteWidth()
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
	if (collected) {
		ItemManager::GetInstance().MarkCollected(this);
	}

	m_collected = collected;
}

bool ShipPart::IsCollected()
{
	return ItemManager::GetInstance().IsCollected(this);
}

void ShipPart::Drop(Vector2 position)
{
}

void ShipPart::Reset()
{
	m_active = false;
	m_collected = false;
}
