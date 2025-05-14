#include "bossenemy.h"

#include "renderer.h"
#include "animatedsprite.h"

#include "logmanager.h"
#include "inlinehelpers.h"

#include <cassert>
#include <cstdlib>

BossEnemy::BossEnemy() : m_sprite(0), m_speed(70.0f), m_wanderDirection(0, 0), m_wanderTime(0), m_wanderDuration(4.0f), m_isPushed(false), m_canSeePlayer(false), m_health(200.0f) {}

BossEnemy::~BossEnemy()
{
	delete m_sprite;
	m_sprite = 0;
}

bool BossEnemy::Initialise(Renderer& renderer, int levelNum)
{
    screenWidth = (float)renderer.GetWidth();
    screenHeight = (float)renderer.GetHeight();

	m_sprite = renderer.CreateAnimatedSprite("..\\assets\\slime.png");

	if (!m_sprite) {
		LogManager::GetInstance().Log("Failed to load boss!");
		return false;
	}

    GetBoss(levelNum);
    m_alive = false;
	m_sprite->SetupFrames(22, 16);
	m_sprite->SetFrameDuration(0.1f);
	SetLoop(true);
	m_sprite->Animate();

    SetAttackDamage(levelNum, m_gameDifficulty);

	return true;
}

void BossEnemy::Process(float deltaTime, Vector2 playerPos)
{
    if (!m_sprite) {
        return;
    }

    if (m_alive) {
        UpdatePushBack(deltaTime);

        if (!m_isPushed) {
            SetColour(m_bossColour[0], m_bossColour[1], m_bossColour[2]);

            Vector2 direction = {
                playerPos.x - m_position.x, playerPos.y - m_position.y
            };

            float length = sqrtf(direction.x * direction.x + direction.y * direction.y);

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
                    m_wanderDirection.x = (float)cos(angle);
                    m_wanderDirection.y = (float)sin(angle);
                }

                m_position.x += m_wanderDirection.x * (m_speed * 0.25f) * deltaTime;
                m_position.y += m_wanderDirection.y * (m_speed * 0.25f) * deltaTime;
            }
        }
        else {
            SetColour(1.0f, 0.0f, 0.0f);
            m_sprite->Restart();
        }

        m_sprite->SetX(static_cast<int>(m_position.x));
        m_sprite->SetY(static_cast<int>(m_position.y));
    }

    m_sprite->Process(deltaTime);
}

void BossEnemy::Draw(Renderer& renderer)
{
    if (!m_sprite) {
        return;
    }

    if (m_alive) {
        m_sprite->Draw(renderer);
    }
}

Vector2& BossEnemy::Position()
{
    return m_position;
}

int BossEnemy::GetSpriteWidth() const
{
    return m_sprite->GetWidth();
}

void BossEnemy::SetActive(bool active)
{
    m_alive = active;
}

GameObject* BossEnemy::Create() const
{
    return new BossEnemy();
}

bool BossEnemy::isActive() const
{
    return m_alive;
}

void BossEnemy::reset()
{
    m_position.x = 0;
    m_position.y = 0;
}

void BossEnemy::SetFrameTime(float time)
{
    m_sprite->SetFrameDuration(time);
}

void BossEnemy::SetLoop(bool loop)
{
    m_sprite->SetLooping(loop);
}

void BossEnemy::SetColour(float red, float green, float blue)
{
    m_sprite->SetRedTint(red);
    m_sprite->SetGreenTint(green);
    m_sprite->SetBlueTint(blue);
}

Vector2 BossEnemy::GetUpdatedPosition(float deltaTime, Vector2 playerPos)
{
    Vector2 m_updatedPos = { 0, 0 };

    Vector2 direction = {
        playerPos.x - m_position.x, playerPos.y - m_position.y
    };

    float length = sqrtf(direction.x * direction.x + direction.y * direction.y);

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
            m_wanderDirection.x = (float)cos(angle);
            m_wanderDirection.y = (float)sin(angle);
        }

        m_updatedPos.x += m_wanderDirection.x * (m_speed * 0.25f) * deltaTime;
        m_updatedPos.y += m_wanderDirection.y * (m_speed * 0.25f) * deltaTime;
    }

    return m_updatedPos;
}

void BossEnemy::ResetWander(Vector2 direction)
{
    m_wanderDirection = direction;
    m_wanderTime = m_wanderDuration;
}

void BossEnemy::ApplyPushBack(Vector2 direction)
{
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y);

    if (length != 0) {
        direction.x /= length;
        direction.y /= length;
    }

    m_pushbackVelocity.x = direction.x * 600.0f;
    m_pushbackVelocity.y = direction.y * 600.0f;
    m_isPushed = true;
}

void BossEnemy::AddDamage(float weaponDamage)
{
    m_health -= weaponDamage;

    if (m_health <= 0.0f) {
        m_alive = false;
    }
}

float BossEnemy::GetDamageDealt()
{
    return m_attackDamage;
}

void BossEnemy::SetAttackDamage(int levelNum, char difficulty)
{
    float damage;
    float multiplier;

    if (levelNum == 5) {
        damage = 35.0f;
    }
    else {
        damage = 30.0f;
    }

    switch (difficulty) {
    case 'E':
        multiplier = 0.5f;
        break;

    case 'N':
        multiplier = 1.0f;
        break;

    case 'H':
        multiplier = 1.5f;
        break;

    default:
        multiplier = 0.5f;
        break;
    }

    m_attackDamage = damage * multiplier;
}

bool BossEnemy::DropsItems()
{
	// 5% chance

    float percent = GetRandomPercentage();

    return percent <= 0.05f;
}

int BossEnemy::GetDroppedItem()
{
	//number of weapons
    int num = GetRandom(0, 3);

    return num;
}

bool BossEnemy::CanSeePlayer()
{
    return m_canSeePlayer;
}

void BossEnemy::SetGameDifficulty(char difficulty)
{
    m_gameDifficulty = difficulty;
}

void BossEnemy::UpdatePushBack(float deltaTime)
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

void BossEnemy::GetBoss(int levelNum)
{
    if (levelNum == 5) {
        InitFinalBoss();
    }
    else {
        InitBoss();
    }
}

void BossEnemy::InitBoss()
{
    m_sprite->SetScale(2.0f);
    m_bossColour[0] = 0.94f;
    m_bossColour[1] = 0.19f;
    m_bossColour[2] = 0.39f;
}

void BossEnemy::InitFinalBoss()
{
    m_sprite->SetScale(4.0f);
    m_bossColour[0] = 0.33f;
    m_bossColour[1] = 0.21f;
    m_bossColour[2] = 0.92f;

    m_position.x = screenWidth / 2.0f;
    m_position.y = screenHeight / 2.0f;
}
