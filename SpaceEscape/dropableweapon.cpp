#include "dropableweapon.h"

DropableWeapon::DropableWeapon() : m_sprite(0), 
{
}

DropableWeapon::~DropableWeapon()
{
}

bool DropableWeapon::Initialise(Renderer& renderer, int weapon)
{
	return false;
}

void DropableWeapon::Process(float deltaTime)
{
}

void DropableWeapon::Draw(Renderer& renderer)
{
}

Vector2& DropableWeapon::Position()
{
	// TODO: insert return statement here
}

int DropableWeapon::GetSpriteWidth() const
{
	return 0;
}

GameObject* DropableWeapon::Create() const
{
	return nullptr;
}

bool DropableWeapon::isActive() const
{
	return false;
}

void DropableWeapon::reset()
{
}

void DropableWeapon::SetActive(bool active)
{
}

void DropableWeapon::SetCollected(bool collected)
{
}

bool DropableWeapon::IsCollected()
{
	return false;
}

void DropableWeapon::Reset()
{
}

void DropableWeapon::Drop(Vector2 position)
{
}

string DropableWeapon::GetFilepath(int weapon)
{
	return string();
}
