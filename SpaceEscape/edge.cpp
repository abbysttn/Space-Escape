#include "edge.h"

#include "renderer.h"
#include "sprite.h"

#include <cassert>
#include <cstdlib>

Edge::Edge() : m_sprite(0) {}

Edge::~Edge()
{
	delete m_sprite;
	m_sprite = 0;
}

bool Edge::initialise(Renderer& renderer, const char* filename)
{
	m_sprite = renderer.CreateSprite(filename);

	m_alive = true;
	m_sprite->SetScale(3.0);

	m_position.x = 0;
	m_position.y = 0;
	return true;
}

void Edge::Process(float deltaTime)
{
	if (m_alive) {
		m_sprite->SetX(static_cast<int>(m_position.x));
		m_sprite->SetY(static_cast<int>(m_position.y));
	}
	m_sprite->Process(deltaTime);
}

void Edge::Draw(Renderer& renderer)
{
	m_sprite->Draw(renderer);
}

Vector2& Edge::Position()
{
	return m_position;
}

int Edge::GetSpriteWidth()
{
	return m_sprite->GetWidth();
}

void Edge::SetColour(float red, float green, float blue)
{
	m_sprite->SetRedTint(red);
	m_sprite->SetGreenTint(green);
	m_sprite->SetBlueTint(blue);
}

GameObject* Edge::Create() const
{
	return new Edge();
}

bool Edge::isActive() const
{
	return m_alive;
}

void Edge::reset()
{
	m_position.x = 0;
	m_position.y = 0;
}

void Edge::SetActive(bool active)
{
	m_alive = active;
}

void Edge::SetRotation(float angle)
{
	m_sprite->SetAngle(angle);
}
