#include "corner.h"

#include "renderer.h"
#include "sprite.h"

#include <cassert>
#include <cstdlib>

Corner::Corner() : m_sprite(0) {}

Corner::~Corner()
{
	delete m_sprite;
	m_sprite = 0;
}

bool Corner::initialise(Renderer& renderer, string& filename)
{
	m_sprite = renderer.CreateSprite(filename.c_str());

	m_alive = true;
	m_sprite->SetScale(1.0);

	m_position.x = 0;
	m_position.y = 0;
	return true;
}

void Corner::Process(float deltaTime)
{
	if (m_alive) {
		m_sprite->SetX(static_cast<int>(m_position.x));
		m_sprite->SetY(static_cast<int>(m_position.y));
	}
	m_sprite->Process(deltaTime);
}

void Corner::Draw(Renderer& renderer)
{
	m_sprite->Draw(renderer);
}

Vector2& Corner::Position()
{
	return m_position;
}

int Corner::GetSpriteWidth()
{
	return m_sprite->GetWidth();
}

void Corner::SetColour(float red, float green, float blue)
{
	m_sprite->SetRedTint(red);
	m_sprite->SetGreenTint(green);
	m_sprite->SetBlueTint(blue);
}

GameObject* Corner::Create() const
{
	return new Corner();
}

bool Corner::isActive() const
{
	return m_alive;
}

void Corner::reset()
{
	m_position.x = 0;
	m_position.y = 0;
}

void Corner::SetActive(bool active)
{
	m_alive = active;
}

void Corner::SetRotation(float angle)
{
	m_sprite->SetAngle(angle);
}
