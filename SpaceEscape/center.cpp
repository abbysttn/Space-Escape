#include "center.h"

#include "renderer.h"
#include "sprite.h"

#include <cassert>
#include <cstdlib>

Center::Center() : m_sprite(0) {}

Center::~Center()
{
	delete m_sprite;
	m_sprite = 0;
}

bool Center::initialise(Renderer& renderer, const char* filename)
{
	m_sprite = renderer.CreateSprite(filename);

	m_alive = true;
	m_sprite->SetScale(3.0);

	m_position.x = 0;
	m_position.y = 0;
	return true;
}

void Center::Process(float deltaTime)
{
	if (m_alive) {
		m_sprite->SetX(static_cast<int>(m_position.x));
		m_sprite->SetY(static_cast<int>(m_position.y));
	}
	m_sprite->Process(deltaTime);
}

void Center::Draw(Renderer& renderer)
{
	m_sprite->Draw(renderer);
}

Vector2& Center::Position()
{
	return m_position;
}

int Center::GetSpriteWidth() const
{
	return m_sprite->GetWidth();
}

void Center::SetColour(float red, float green, float blue)
{
	m_sprite->SetRedTint(red);
	m_sprite->SetGreenTint(green);
	m_sprite->SetBlueTint(blue);
}

GameObject* Center::Create() const
{
	return new Center();
}

bool Center::isActive() const
{
	return m_alive;
}

void Center::reset()
{
	m_position.x = 0;
	m_position.y = 0;
}

void Center::SetActive(bool active)
{
	m_alive = active;
}

void Center::SetRotation(float angle)
{
	m_sprite->SetAngle(angle);
}
