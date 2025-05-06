#include "bullet.h"

#include "renderer.h"
#include "sprite.h"

#include <math.h>

#include <cassert>
#include <cstdlib>

Bullet::Bullet() : m_sprite(0), m_speed(100.0f), m_targetPos(0,0) {}

Bullet::~Bullet()
{
	delete m_sprite;
	m_sprite = 0;
}

bool Bullet::initialise(Renderer& renderer)
{
	m_sprite = renderer.CreateSprite("..\\assets\\bullet.png");

	m_alive = false;
	m_sprite->SetScale(0.5f);

	m_position.x = 0;
	m_position.y = 0;

	float width = renderer.GetWidth();
	float height = renderer.GetHeight();

	m_boundsLow.x = m_sprite->GetWidth() / 2.0f;
	m_boundsLow.y = m_sprite->GetHeight() / 2.0f;
	m_boundsHigh.x = width - (m_sprite->GetWidth() / 2.0f);
	m_boundsHigh.y = height - (m_sprite->GetHeight() / 2.0f);

	return true;
}

void Bullet::Process(float deltaTime)
{
	if (m_alive) {
		m_position.x += m_velocity.x * deltaTime;
		m_position.y += m_velocity.y * deltaTime;

		m_sprite->SetX(static_cast<int>(m_position.x));
		m_sprite->SetY(static_cast<int>(m_position.y));
	}

	if (m_position.x <= m_boundsLow.x || m_position.y <= m_boundsLow.y || m_position.x >= m_boundsHigh.x || m_position.y >= m_boundsHigh.y) {
		m_alive = false;
	}

	m_sprite->Process(deltaTime);
}

void Bullet::Draw(Renderer& renderer)
{
	if (m_alive) {
		m_sprite->Draw(renderer);
	}
}

Vector2& Bullet::Position()
{
	return m_position;
}

int Bullet::GetSpriteWidth()
{
	return m_sprite->GetWidth();
}

void Bullet::SetColour(float red, float green, float blue)
{
	m_sprite->SetRedTint(red);
	m_sprite->SetGreenTint(green);
	m_sprite->SetBlueTint(blue);
}

GameObject* Bullet::Create() const
{
	return new Bullet();
}

bool Bullet::isActive() const
{
	return m_alive;
}

void Bullet::reset()
{
	m_position.x = 0;
	m_position.y = 0;
}

void Bullet::SetActive(bool active)
{
	m_alive = active;
}

void Bullet::Fire(Vector2& startPos, const Vector2& targetPos)
{
	m_position = startPos;
	m_targetPos = targetPos;
	m_alive = true;

	Vector2 direction = {
		m_targetPos.x - m_position.x, m_targetPos.y - m_position.y
	};

	float length = sqrt(direction.x * direction.x + direction.y * direction.y);

	if (length != 0.0f) {
		direction.x /= length;
		direction.y /= length;
	}

	m_velocity.x = direction.x * m_speed;
	m_velocity.y = direction.y * m_speed;
}
