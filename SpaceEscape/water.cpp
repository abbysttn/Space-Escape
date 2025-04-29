#include "water.h"

#include "renderer.h"
#include "sprite.h"

#include <cassert>
#include <cstdlib>

Water::Water() : m_sprite(0) {}

Water::~Water()
{
	delete m_sprite;
	m_sprite = 0;
}

bool Water::initialise(Renderer& renderer, const char* filename)
{
	m_sprite = renderer.CreateSprite(filename);

	m_alive = true;
	m_sprite->SetScale(3.0);

	m_position.x = 0;
	m_position.y = 0;
	return true;
}

void Water::Process(float deltaTime)
{
	if (m_alive) {
		m_sprite->SetX(static_cast<int>(m_position.x));
		m_sprite->SetY(static_cast<int>(m_position.y));
	}
	m_sprite->Process(deltaTime);
}

void Water::Draw(Renderer& renderer)
{
	m_sprite->Draw(renderer);
}

Vector2& Water::Position()
{
	return m_position;
}

int Water::GetSpriteWidth()
{
	return m_sprite->GetWidth();
}

void Water::SetColour(float red, float green, float blue)
{
	m_sprite->SetRedTint(red);
	m_sprite->SetGreenTint(green);
	m_sprite->SetBlueTint(blue);
}

GameObject* Water::Create() const
{
	return new Water();
}

bool Water::isActive() const
{
	return m_alive;
}

void Water::reset()
{
	m_position.x = 0;
	m_position.y = 0;
}

void Water::SetActive(bool active)
{
	m_alive = active;
}
