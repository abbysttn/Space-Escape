#include "player.h"

#include "renderer.h"
#include "animatedsprite.h"

#include <cassert>
#include <cstdlib>

Player::Player() : m_sprite(0) {}

Player::~Player()
{
	delete m_sprite;
	m_sprite = 0;
}

bool Player::Initialise(Renderer& renderer, const char* filename)
{
    m_sprite = renderer.CreateAnimatedSprite(filename);
    
    if (!m_sprite) {
        return false;
    }

    m_position.x = 800;
    m_position.y = 500;

    m_sprite->SetScale(3.0f);
    m_sprite->SetupFrames(16, 16);
    m_sprite->Animate();

    return true;
}

void Player::Process(float deltaTime)
{
    if (!m_sprite) {
        return;
    }

    if (m_alive) {
        m_sprite->SetX(static_cast<int>(m_position.x));
        m_sprite->SetY(static_cast<int>(m_position.y));
    }

    m_sprite->Process(deltaTime);
}

void Player::Draw(Renderer& renderer)
{
    if (!m_sprite) {
        return;
    }

    if (m_alive) {
        m_sprite->Draw(renderer);
    }
}

Vector2& Player::Position()
{
    return m_position;
}

int Player::GetSpriteWidth()
{
    return 16;
}

void Player::SetColour(float red, float green, float blue)
{
    m_sprite->SetRedTint(red);
    m_sprite->SetGreenTint(green);
    m_sprite->SetBlueTint(blue);
}

void Player::SetActive(bool active)
{
    m_alive = active;
}

void Player::SetRotation(float angle)
{
    m_sprite->SetAngle(angle);
}

GameObject* Player::Create() const
{
    return new Player();
}

bool Player::isActive() const
{
    return m_alive;
}

void Player::reset()
{
    m_position.x = 0;
    m_position.y = 0;
}

void Player::SetFrameTime(float time)
{
    m_sprite->SetFrameDuration(time);
}

void Player::SetLoop(bool loop)
{
    m_sprite->SetLooping(loop);
}

bool Player::IsRunning()
{
    return m_sprite->IsAnimating();
}

void Player::SetRunning()
{
    m_sprite->Animate();
    m_sprite->Restart();
}
