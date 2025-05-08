#include "player.h"

#include "renderer.h"
#include "animatedsprite.h"

#include <cassert>
#include <cstdlib>





#include "logmanager.h"

Player::Player() : m_sprite(0), m_isPushed(false), m_health(100.0f), m_lives(3), m_pushbackComplete(true) {}

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
        if (m_isPushed) {
            UpdatePushBack(deltaTime);
            SetColour(1.0f, 0.0f, 0.0f);
        }
        else {
            SetColour(1.0f, 1.0f, 1.0f);
        }

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
    return m_sprite->GetWidth();
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

void Player::ApplyPushBack(Vector2 direction)
{
    float length = sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length != 0) {
        direction.x /= length;
        direction.y /= length;
    }

    m_pushbackVelocity.x = direction.x * 400.0f;
    m_pushbackVelocity.y = direction.y * 400.0f;
    m_isPushed = true;
    m_pushbackComplete = false;
}

bool Player::IsPushedBack()
{
    return m_isPushed;
}

bool Player::IsPushBackComplete()
{
    return m_pushbackComplete;
}

Vector2 Player::GetUpdatedPushPosition(float deltaTime)
{
    Vector2 updatedPos = m_position;

    updatedPos.x += m_pushbackVelocity.x * deltaTime;
    updatedPos.y += m_pushbackVelocity.y * deltaTime;

    return updatedPos;
}

void Player::SetPushedBack(bool pushed)
{
    m_isPushed = pushed;
}

void Player::AddDamage(float damage)
{
    m_health -= damage;

    if (m_health <= 0.0f) {
        m_lives--;
        m_health = 100.0f;
        LogManager::GetInstance().Log("Lost A Life!");
        if (m_lives <= 0) {
            LogManager::GetInstance().Log("All Lives Lost!");
            m_alive = false;
        }
    }
}

void Player::UpdatePushBack(float deltaTime)
{
    if (m_isPushed) {
        Vector2 originalPos = m_position;

        m_position.x += m_pushbackVelocity.x * deltaTime;
        m_position.y += m_pushbackVelocity.y * deltaTime;

        m_pushbackVelocity.x *= 0.9f;
        m_pushbackVelocity.y *= 0.9f;

        if (fabs(m_pushbackVelocity.x) < 0.01f && fabs(m_pushbackVelocity.y) < 0.1f) {
            m_pushbackVelocity = { 0, 0 };
            m_isPushed = false;
            m_pushbackComplete = true;
        }
    }
}
