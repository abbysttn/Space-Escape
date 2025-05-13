#include "playership.h"

#include "renderer.h"
#include "sprite.h"

#include <cassert>
#include <cstdlib>

PlayerShip::PlayerShip() : m_sprite(0), m_active(true) {}

PlayerShip::~PlayerShip()
{
	delete m_sprite;
	m_sprite = 0;
}

bool PlayerShip::Initialise(Renderer& renderer)
{
	m_sprite = renderer.CreateSprite("..\\assets\\player_ship.png");

	m_sprite->SetScale(1.0f);

	float width = (float)renderer.GetWidth();
	float height = (float)renderer.GetHeight();

	m_position.x = 10.0f;
	m_position.y = height / 2.0f;

	m_boundsLow.x = m_sprite->GetWidth() / 2.0f;
	m_boundsLow.y = m_sprite->GetHeight() / 2.0f;
	m_boundsHigh.x = width - (m_sprite->GetWidth() / 2.0f);
	m_boundsHigh.y = height - (m_sprite->GetHeight() / 2.0f);

	return true;
}

void PlayerShip::Process(float deltaTime)
{
	if (m_active) {
		m_sprite->SetX(static_cast<int>(m_position.x));
		m_sprite->SetY(static_cast<int>(m_position.y));
	}

	if (m_position.x >= m_boundsHigh.x || m_position.y >= m_boundsHigh.y) {
		m_active = false;
	}

	m_sprite->Process(deltaTime);
}

void PlayerShip::Draw(Renderer& renderer)
{
	if (m_active) {
		m_sprite->Draw(renderer);
	}
}

Vector2& PlayerShip::Position()
{
	return m_position;
}

void PlayerShip::SetRotation(float angle)
{
	m_sprite->SetAngle(angle);
}

void PlayerShip::SetActive(bool active)
{
	m_active = active;
}

int PlayerShip::GetSpriteWidth()
{
	return m_sprite->GetWidth();
}

void PlayerShip::Fly(float deltaTime)
{
	m_position.x += 150.0f * deltaTime;
}

void PlayerShip::Fall(float deltaTime)
{
	m_position.x += 200.0f * deltaTime;
	m_position.y += 150.0f * deltaTime;
}

bool PlayerShip::IsActive()
{
	return m_active;
}
