#include "underlayTiles.h"

#include "renderer.h"
#include "sprite.h"

#include <cassert>
#include <cstdlib>

UnderlayTiles::UnderlayTiles() : m_sprite(0) {}

UnderlayTiles::~UnderlayTiles()
{
	delete m_sprite;
	m_sprite = 0;
}

bool UnderlayTiles::initialise(Renderer& renderer, const char* filename)
{
	m_sprite = renderer.CreateSprite(filename);

	m_alive = true;
	m_sprite->SetScale(3.0);

	m_position.x = 0;
	m_position.y = 0;
	return true;
}

void UnderlayTiles::Process(float deltaTime)
{
	if (m_alive) {
		m_sprite->SetX(static_cast<int>(m_position.x));
		m_sprite->SetY(static_cast<int>(m_position.y));
	}
	m_sprite->Process(deltaTime);
}

void UnderlayTiles::Draw(Renderer& renderer)
{
	m_sprite->Draw(renderer);
}

Vector2& UnderlayTiles::Position()
{
	return m_position;
}

int UnderlayTiles::GetSpriteWidth() const
{
	return m_sprite->GetWidth();
}

void UnderlayTiles::SetColour(float red, float green, float blue)
{
	m_sprite->SetRedTint(red);
	m_sprite->SetGreenTint(green);
	m_sprite->SetBlueTint(blue);
}

GameObject* UnderlayTiles::Create() const
{
	return new UnderlayTiles();
}

bool UnderlayTiles::isActive() const
{
	return m_alive;
}

void UnderlayTiles::reset()
{
	m_position.x = 0;
	m_position.y = 0;
}

void UnderlayTiles::SetActive(bool active)
{
	m_alive = active;
}

void UnderlayTiles::SetRotation(float angle)
{
	m_sprite->SetAngle(angle);
}
