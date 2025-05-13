#include "edgeCorner.h"

#include "renderer.h"
#include "sprite.h"

#include <cassert>
#include <cstdlib>

EdgeCorner::EdgeCorner() : m_sprite(0) {}

EdgeCorner::~EdgeCorner()
{
	delete m_sprite;
	m_sprite = 0;
}

bool EdgeCorner::initialise(Renderer& renderer, const char* filename)
{
	m_sprite = renderer.CreateSprite(filename);

	m_alive = true;
	m_sprite->SetScale(3.0);

	m_position.x = 0;
	m_position.y = 0;
	return true;
}

void EdgeCorner::Process(float deltaTime)
{
	if (m_alive) {
		m_sprite->SetX(static_cast<int>(m_position.x));
		m_sprite->SetY(static_cast<int>(m_position.y));
	}
	m_sprite->Process(deltaTime);
}

void EdgeCorner::Draw(Renderer& renderer)
{
	m_sprite->Draw(renderer);
}

Vector2& EdgeCorner::Position()
{
	return m_position;
}

int EdgeCorner::GetSpriteWidth() const
{
	return m_sprite->GetWidth();
}

void EdgeCorner::SetColour(float red, float green, float blue)
{
	m_sprite->SetRedTint(red);
	m_sprite->SetGreenTint(green);
	m_sprite->SetBlueTint(blue);
}

GameObject* EdgeCorner::Create() const
{
	return new EdgeCorner();
}

bool EdgeCorner::isActive() const
{
	return m_alive;
}

void EdgeCorner::reset()
{
	m_position.x = 0;
	m_position.y = 0;
}

void EdgeCorner::SetActive(bool active)
{
	m_alive = active;
}

void EdgeCorner::SetRotation(float angle)
{
	m_sprite->SetAngle(angle);
}
