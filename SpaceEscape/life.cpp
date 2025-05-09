#include "life.h"

#include "renderer.h"
#include "sprite.h"

#include <cassert>
#include <cstdlib>

Life::Life() : m_sprite(0) {}

Life::~Life()
{
	delete m_sprite;
	m_sprite = 0;
}

bool Life::initialise(Renderer& renderer)
{
	m_sprite = renderer.CreateSprite("..\\assets\\life.png");

	m_alive = true;
	m_sprite->SetScale(3.0);

	m_position.x = 0;
	m_position.y = 0;
	return true;
}

void Life::Process(float deltaTime)
{
	m_sprite->SetX(static_cast<int>(m_position.x));
	m_sprite->SetY(static_cast<int>(m_position.y));
	m_sprite->Process(deltaTime);
}

void Life::Draw(Renderer& renderer)
{
	m_sprite->Draw(renderer);
}

Vector2& Life::Position()
{
	return m_position;
}

int Life::GetSpriteWidth()
{
	return m_sprite->GetWidth();
}

void Life::SetColour(float red, float green, float blue)
{
	m_sprite->SetRedTint(red);
	m_sprite->SetGreenTint(green);
	m_sprite->SetBlueTint(blue);
}

GameObject* Life::Create() const
{
	return new Life();
}

bool Life::isActive() const
{
	return m_alive;
}

void Life::reset()
{
	m_position.x = 0;
	m_position.y = 0;
}

void Life::SetActive(bool active)
{
	m_alive = active;
}
