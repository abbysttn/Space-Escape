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

bool Prop::initialise(Renderer& renderer, string levelType)
{
	int num = GetRandom(1, 6);

	m_sprite = renderer.CreateSprite(GetFilePath(num, levelType).c_str());

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

string Prop::GetFilePath(int num, std::string leveltype)
{
	string filepath = "..\\assets\\";

	switch (num) {
	case 1:
		filepath += "rock.png";
		break;

	case 2:
		filepath += "grass_1_" + leveltype + ".png";
		break;

	case 3:
		filepath += "grass_2_" + leveltype + ".png";
		break;

	case 4:
		filepath += "grass_3_" + leveltype + ".png";
		break;

	case 5:
		filepath += "scar_1.png";
		break;

	case 6:
		filepath += "scar_2.png";
		break;

	default:
		filepath += "rock.png";
		break;
	}

	return filepath;
}
