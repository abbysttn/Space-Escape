#include "selectionarrow.h"

#include "renderer.h"
#include "animatedsprite.h"
#include <string>

#include <cassert>
#include <cstdlib>

SelectionArrow::SelectionArrow() : m_sprite(0) {}

SelectionArrow::~SelectionArrow()
{
	delete m_sprite;
	m_sprite = 0;
}

bool SelectionArrow::initialise(Renderer& renderer, char direction)
{
	m_direction = direction;

	std::string filepath = "..\\assets\\";

	if (direction == 'L') {
		filepath += "arrow_left.png";
	}
	else if (direction == 'R') {
		filepath += "arrow_right.png";
	}

	m_sprite = renderer.CreateAnimatedSprite(filepath.c_str());

	m_sprite->SetScale(5.0);
	m_sprite->SetupFrames(17, 31);
	m_sprite->SetFrameDuration(0.1f);
	m_sprite->SetLooping(false);

	m_position.x = 550;
	m_position.y = 550;
	return true;
}

void SelectionArrow::Process(float deltaTime)
{
	if (!m_sprite->IsAnimating()) {
		ResetButtonState();
	}

	m_sprite->SetX(static_cast<int>(m_position.x));
	m_sprite->SetY(static_cast<int>(m_position.y));
	m_sprite->Process(deltaTime);
}

void SelectionArrow::Draw(Renderer& renderer)
{
	m_sprite->Draw(renderer);
}

Vector2& SelectionArrow::Position()
{
	return m_position;
}

int SelectionArrow::GetSpriteWidth()
{
	return m_sprite->GetWidth();
}

int SelectionArrow::GetSpriteHeight()
{
	return m_sprite->GetHeight();
}

GameObject* SelectionArrow::Create() const
{
	return new SelectionArrow();
}

bool SelectionArrow::isActive() const
{
	return true;
}

void SelectionArrow::reset()
{
	m_position.x = 0;
	m_position.y = 0;
}

void SelectionArrow::SetOffset(float offset)
{
	switch (m_direction) {
	case 'L':
		m_position.x += offset / 1.5f;
		break;

	case 'R':
		m_position.x -= offset / 1.5f;
		break;
	}
}

void SelectionArrow::Pressed()
{
	m_sprite->Animate();
}

void SelectionArrow::ResetButtonState()
{
	m_sprite->Restart();
}
