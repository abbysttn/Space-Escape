#include "button.h"

#include "renderer.h"
#include "animatedsprite.h"

#include <cassert>
#include <cstdlib>

Button::Button() : m_sprite(0) {}

Button::~Button()
{
	delete m_sprite;
	m_sprite = 0;
}

bool Button::initialise(Renderer& renderer)
{
	m_sprite = renderer.CreateAnimatedSprite("..\\assets\\button_long.png");

	m_sprite->SetScale(5.0);
	m_sprite->SetupFrames(150, 22);
	m_sprite->SetFrameDuration(0.1f);
	m_sprite->SetLooping(false);

	m_position.x = 0;
	m_position.y = 0;
	return true;
}

void Button::Process(float deltaTime)
{
	if (!m_sprite->IsAnimating()) {
		ResetButtonState();
	}

	m_sprite->SetX(static_cast<int>(m_position.x));
	m_sprite->SetY(static_cast<int>(m_position.y));
	m_sprite->Process(deltaTime);
}

void Button::Draw(Renderer& renderer)
{
	m_sprite->Draw(renderer);
}

Vector2& Button::Position()
{
	return m_position;
}

int Button::GetSpriteWidth()
{
	return m_sprite->GetWidth();
}

int Button::GetSpriteHeight()
{
	return m_sprite->GetHeight();
}

GameObject* Button::Create() const
{
	return new Button();
}

bool Button::isActive() const
{
	return true;
}

void Button::reset()
{
	m_position.x = 0;
	m_position.y = 0;
}

void Button::ResetButtonState()
{
	m_sprite->Restart();
}

void Button::Pressed()
{
	m_sprite->Animate();
}
