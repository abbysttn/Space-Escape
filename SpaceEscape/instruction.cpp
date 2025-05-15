#include "instruction.h"

#include "Renderer.h"
#include "sprite.h"

#include <cassert>
#include <cstdlib>

Instruction::Instruction() : m_sprite(0) {}

Instruction::~Instruction()
{
	delete m_sprite;
	m_sprite = 0;
}

bool Instruction::initialise(Renderer& renderer, string type)
{
	string filepath = "..\\assets\\" + type + "_instructions.png";

	m_sprite = renderer.CreateSprite(filepath.c_str());

	m_sprite->SetScale(2.0);
	m_alive = true;

	m_position.x = 0;
	m_position.y = 0;
	return true;
}

void Instruction::Process(float deltaTime)
{
	m_sprite->SetX(static_cast<int>(m_position.x));
	m_sprite->SetY(static_cast<int>(m_position.y));
	m_sprite->Process(deltaTime);
}

void Instruction::Draw(Renderer& renderer)
{
	m_sprite->Draw(renderer);
}

Vector2& Instruction::Position()
{
	return m_position;
}

int Instruction::GetSpriteWidth() const
{
	return m_sprite->GetWidth();
}

GameObject* Instruction::Create() const
{
	return new Instruction();
}

bool Instruction::isActive() const
{
	return m_alive;
}

void Instruction::reset()
{
	m_position.x = 0;
	m_position.y = 0;
}
