#include "prop.h"

#include "renderer.h"
#include "sprite.h"

#include "inlinehelpers.h"

#include <cassert>
#include <cstdlib>

Prop::Prop() : m_sprite(0) {}

Prop::~Prop()
{
	delete m_sprite;
	m_sprite = 0;
}

bool Prop::initialise(Renderer& renderer, const char* filename)
{
	m_sprite = renderer.CreateSprite(filename);

	m_alive = true;
	m_sprite->SetScale(3.0);

	m_position.x = 0;
	m_position.y = 0;
	return true;
}

void Prop::Process(float deltaTime)
{
	if (m_alive) {
		m_sprite->SetX(static_cast<int>(m_position.x));
		m_sprite->SetY(static_cast<int>(m_position.y));
	}
	m_sprite->Process(deltaTime);
}

void Prop::Draw(Renderer& renderer)
{
	m_sprite->Draw(renderer);
}

Vector2& Prop::Position()
{
	return m_position;
}

int Prop::GetSpriteWidth()
{
	return m_sprite->GetWidth();
}

void Prop::SetColour(float red, float green, float blue)
{
	m_sprite->SetRedTint(red);
	m_sprite->SetGreenTint(green);
	m_sprite->SetBlueTint(blue);
}

GameObject* Prop::Create() const
{
	return new Prop();
}

bool Prop::isActive() const
{
	return m_alive;
}

void Prop::reset()
{
	m_position.x = 0;
	m_position.y = 0;
}

void Prop::SetActive(bool active)
{
	m_alive = active;
}

void Prop::SetRotation(float angle)
{
	m_sprite->SetAngle(angle);
}

const char* Prop::GetFilePath(int num, std::string leveltype)
{
	switch (num) {
	case 1:

	case 2:

	case 3:

	case 4:

	case 5:

	case 6:
	}
}
