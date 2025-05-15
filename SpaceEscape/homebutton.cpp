#include "homebutton.h"

#include "renderer.h"
#include "animatedsprite.h"

#include <cassert>
#include <cstdlib>

HomeButton::HomeButton() : m_sprite(0) {}

HomeButton::~HomeButton()
{
	delete m_sprite;
	m_sprite = 0;
}

bool HomeButton::initialise(Renderer& renderer)
{
	m_sprite = renderer.CreateAnimatedSprite("..\\assets\\menu_button.png");

	m_sprite->SetScale(1.0);
	m_sprite->SetupFrames(48, 48);
	m_sprite->SetFrameDuration(0.1f);
	m_sprite->SetLooping(false);

	m_position.x = 0;
	m_position.y = 0;
	return true;
}

void HomeButton::Process(float deltaTime)
{
	if (!m_sprite->IsAnimating()) {
		ResetButtonState();
	}

	m_sprite->SetX(static_cast<int>(m_position.x));
	m_sprite->SetY(static_cast<int>(m_position.y));
	m_sprite->Process(deltaTime);
}

void HomeButton::Draw(Renderer& renderer)
{
	m_sprite->Draw(renderer);
}

Vector2& HomeButton::Position()
{
	return m_position;
}

int HomeButton::GetSpriteWidth() const
{
	return m_sprite->GetWidth();
}

int HomeButton::GetSpriteHeight()
{
	return m_sprite->GetHeight();
}

GameObject* HomeButton::Create() const
{
	return new HomeButton();
}

bool HomeButton::isActive() const
{
	return true;
}

void HomeButton::reset()
{
	m_position.x = 0;
	m_position.y = 0;
}

void HomeButton::Pressed()
{
	m_sprite->Animate();
}

void HomeButton::ResetButtonState()
{
	m_sprite->Restart();
}
