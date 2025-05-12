#include "riftvial.h"

#include "renderer.h"
#include "sprite.h"

#include <cassert>
#include <cstdlib>

RiftVial::RiftVial() : m_sprite(0), m_isCollected(false) {}

RiftVial::~RiftVial()
{
	delete m_sprite;
	m_sprite = 0;
}

bool RiftVial::Initialise(Renderer& renderer)
{
	m_sprite = renderer.CreateSprite("..\\assets\\rift_vial.png");

	m_sprite->SetScale(3.0);

	m_position.x = 0;
	m_position.y = 0;
	return true;
}

void RiftVial::Process(float deltaTime)
{
	m_sprite->SetX(static_cast<int>(m_position.x));
	m_sprite->SetY(static_cast<int>(m_position.y));
	m_sprite->Process(deltaTime);
}

void RiftVial::Draw(Renderer& renderer)
{
	if (!m_isCollected) {
		m_sprite->Draw(renderer);
	}
}

Vector2& RiftVial::Position()
{
	return m_position;
}

int RiftVial::GetSpriteWidth()
{
	return m_sprite->GetWidth();
}

void RiftVial::SetRotation(float angle)
{
	m_sprite->SetAngle(angle);
}

void RiftVial::SetCollected(bool collected)
{
	m_isCollected = collected;
}

bool RiftVial::IsCollected()
{
	return m_isCollected;
}

void RiftVial::Drop(Vector2 position)
{
	m_position = position;
}
