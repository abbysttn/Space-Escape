#include "bridge.h"

#include "renderer.h"
#include "sprite.h"

#include <cassert>
#include <cstdlib>

Bridge::Bridge() : m_sprite(0) {}

Bridge::~Bridge()
{
	delete m_sprite;
	m_sprite = 0;
}

bool Bridge::initialise(Renderer& renderer, const char* filename)
{
	m_sprite = renderer.CreateSprite(filename);

	m_alive = true;
	m_sprite->SetScale(3.0);

	m_position.x = 0;
	m_position.y = 0;
	return true;
}

void Bridge::Process(float deltaTime)
{
	if (m_alive) {
		m_sprite->SetX(static_cast<int>(m_position.x));
		m_sprite->SetY(static_cast<int>(m_position.y));
	}
	m_sprite->Process(deltaTime);
}

void Bridge::Draw(Renderer& renderer)
{
	m_sprite->Draw(renderer);
}

Vector2& Bridge::Position()
{
	return m_position;
}

int Bridge::GetSpriteWidth()
{
	return m_sprite->GetWidth();
}

void Bridge::SetColour(float red, float green, float blue)
{
	m_sprite->SetRedTint(red);
	m_sprite->SetGreenTint(green);
	m_sprite->SetBlueTint(blue);
}

GameObject* Bridge::Create() const
{
	return new Bridge();
}

bool Bridge::isActive() const
{
	return m_alive;
}

void Bridge::reset()
{
	m_position.x = 0;
	m_position.y = 0;
}

void Bridge::SetActive(bool active)
{
	m_alive = active;
}

void Bridge::SetRotation(float angle)
{
	m_sprite->SetAngle(angle);
}
