#include "enemyspawner.h"

#include "renderer.h"
#include "sprite.h"

#include <cassert>
#include <cstdlib>

EnemySpawner::EnemySpawner() : m_sprite(0) {}

EnemySpawner::~EnemySpawner()
{
	delete m_sprite;
	m_sprite = 0;
}

bool EnemySpawner::Initialise(Renderer& renderer)
{
	m_sprite = renderer.CreateSprite("..\\assets\\spawner.png");

	m_alive = true;
	m_sprite->SetScale(3.0);

	m_position.x = 500;
	m_position.y = 500;
	return true;
}

void EnemySpawner::Process(float deltaTime)
{
	if (m_alive) {
		m_sprite->SetX(static_cast<int>(m_position.x));
		m_sprite->SetY(static_cast<int>(m_position.y));
	}
	m_sprite->Process(deltaTime);
}

void EnemySpawner::Draw(Renderer& renderer)
{
	m_sprite->Draw(renderer);
}

Vector2& EnemySpawner::Position()
{
	return m_position;
}

int EnemySpawner::GetSpriteWidth()
{
	return m_sprite->GetWidth();
}

GameObject* EnemySpawner::Create() const
{
	return new EnemySpawner();
}

bool EnemySpawner::isActive() const
{
	return m_alive;
}

void EnemySpawner::reset()
{
	m_position.x = 0;
	m_position.y = 0;
}

void EnemySpawner::SetActive(bool active)
{
	m_alive = active;
}
