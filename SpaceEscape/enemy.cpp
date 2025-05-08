#include "enemy.h"

#include "renderer.h"
#include "animatedsprite.h"

#include "logmanager.h"
#include "inlinehelpers.h"

#include <cassert>
#include <cstdlib>
#include <math.h>

Enemy::Enemy() : m_sprite(0), m_speed(50.0f), m_wanderDirection(0, 0), m_wanderTime(0), m_wanderDuration(2.0f), m_isPushed(false), m_canSeePlayer(false), m_health(100.0f) {}

Enemy::~Enemy()
{
	delete m_sprite;
	m_sprite = 0;
}

bool Enemy::Initialise(Renderer& renderer, const char* filename)
{
    m_sprite = renderer.CreateAnimatedSprite(filename);

    if (!m_sprite) {
        LogManager::GetInstance().Log("Failed to load enemy!");
        return false;
    }

    m_sprite->SetScale(1.5f);
    m_sprite->SetupFrames(24, 24);
    m_sprite->SetFrameDuration(0.4f);
    SetLoop(true);
    m_sprite->Animate();

    return true;
}

void Enemy::Process(float deltaTime, Vector2 playerPos)
{
    if (!m_sprite) {
        return;
    }

    if (m_alive) {
        UpdatePushBack(deltaTime);

        if (!m_isPushed) {
            SetColour(1.0f, 1.0f, 1.0f);

            Vector2 direction = {
                playerPos.x - m_position.x, playerPos.y - m_position.y
            };

            float length = sqrt(direction.x * direction.x + direction.y * direction.y);

            if (length <= 150.0f) {
                m_canSeePlayer = true;

                if (length != 0.0f) {
                    direction.x /= length;
                    direction.y /= length;
                }

                m_velocity.x = direction.x * m_speed;
                m_velocity.y = direction.y * m_speed;

                m_position.x += m_velocity.x * deltaTime;
                m_position.y += m_velocity.y * deltaTime;
            }
            else {
                m_canSeePlayer = false;

                m_wanderTime -= deltaTime;

                if (m_wanderTime <= 0) {
                    m_wanderTime = m_wanderDuration;

                    float angle = GetRandom(0, 360) * (3.14519f / 180.0f); //angle in radians
                    m_wanderDirection.x = cos(angle);
                    m_wanderDirection.y = sin(angle);
                }

                m_position.x += m_wanderDirection.x * (m_speed * 0.25f) * deltaTime;
                m_position.y += m_wanderDirection.y * (m_speed * 0.25f) * deltaTime;
            }
        }
        else {
            SetColour(1.0f, 0.0f, 0.0f);
        }

        m_sprite->SetX(static_cast<int>(m_position.x));
        m_sprite->SetY(static_cast<int>(m_position.y));
    }

    m_sprite->Process(deltaTime);
}

void Enemy::Draw(Renderer& renderer)
{
    if (!m_sprite) {
        return;
    }

    if (m_alive) {
        m_sprite->Draw(renderer);
    }
}

Vector2& Enemy::Position()
{
    return m_position;
}

int Enemy::GetSpriteWidth()
{
    return m_sprite->GetWidth();
}

void Enemy::SetColour(float red, float green, float blue)
{
    m_sprite->SetRedTint(red);
    m_sprite->SetGreenTint(green);
    m_sprite->SetBlueTint(blue);
}

void Enemy::SetActive(bool active)
{
    m_alive = active;
}

void Enemy::SetRotation(float angle)
{
    m_sprite->SetAngle(angle);
}

GameObject* Enemy::Create() const
{
	return new Enemy();
}

bool Enemy::isActive() const
{
    return m_alive;
}

void Enemy::reset()
{
    m_position.x = 0;
    m_position.y = 0;
}

void Enemy::SetFrameTime(float time)
{
    m_sprite->SetFrameDuration(time);
}

void Enemy::SetLoop(bool loop)
{
    m_sprite->SetLooping(loop);
}

bool Enemy::IsRunning()
{
    return m_sprite->IsAnimating();
}

void Enemy::SetRunning()
{
    m_sprite->Animate();
    m_sprite->Restart();
}

Vector2 Enemy::GetUpdatedPosition(float deltaTime, Vector2 playerPos)
{
    Vector2 m_updatedPos = { 0, 0 };

    Vector2 direction = {
        playerPos.x - m_position.x, playerPos.y - m_position.y
    };

    float length = sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length <= 150.0f) {
        m_canSeePlayer = true;

        if (length != 0.0f) {
            direction.x /= length;
            direction.y /= length;
        }

        m_velocity.x = direction.x * m_speed;
        m_velocity.y = direction.y * m_speed;

        m_updatedPos.x += m_velocity.x * deltaTime;
        m_updatedPos.y += m_velocity.y * deltaTime;
    }
    else {
        m_canSeePlayer = false;

        m_wanderTime -= deltaTime;

        if (m_wanderTime <= 0) {
            m_wanderTime = m_wanderDuration;

            float angle = GetRandom(0, 360) * (3.14519f / 180.0f); //angle in radians
            m_wanderDirection.x = cos(angle);
            m_wanderDirection.y = sin(angle);
        }

        m_updatedPos.x += m_wanderDirection.x * (m_speed * 0.25f) * deltaTime;
        m_updatedPos.y += m_wanderDirection.y * (m_speed * 0.25f) * deltaTime;
    }

    return m_updatedPos;
}

void Enemy::ResetWander(Vector2 direction)
{
    m_wanderDirection = direction;
    m_wanderTime = m_wanderDuration;
}

void Enemy::ApplyPushBack(Vector2 direction)
{
    float length = sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length != 0) {
        direction.x /= length;
        direction.y /= length;
    }

    m_pushbackVelocity.x = direction.x * 600.0f;
    m_pushbackVelocity.y = direction.y * 600.0f;
    m_isPushed = true;
}

void Enemy::AddDamage(float weaponDamage)
{
    m_health -= weaponDamage;

    if (m_health <= 0.0f) {
        m_alive = false;
    }
}

void Enemy::UpdatePushBack(float deltaTime)
{
    if (m_isPushed) {
        m_position.x += m_pushbackVelocity.x * deltaTime;
        m_position.y += m_pushbackVelocity.y * deltaTime;

        m_pushbackVelocity.x *= 0.9f;
        m_pushbackVelocity.y *= 0.9f;

        if (fabs(m_pushbackVelocity.x) < 0.01f && fabs(m_pushbackVelocity.y) < 0.1f) {
            m_pushbackVelocity = { 0, 0 };
            m_isPushed = false;
        }
    }
}
