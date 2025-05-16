#include "playerstate.h"

#include "logmanager.h"
#include <string>

PlayerState* PlayerState::sm_pInstance = 0;

PlayerState& PlayerState::GetInstance()
{
    if (sm_pInstance == 0) {
        sm_pInstance = new PlayerState();
    }

    return (*sm_pInstance);
}

void PlayerState::DestroyInstance()
{
    delete sm_pInstance;
    sm_pInstance = 0;
}

void PlayerState::DebugDraw()
{
}

float PlayerState::GetHealth()
{
    return m_currentHealth;
}

float PlayerState::GetMaxHealth()
{
    return m_maxHealth;
}

int PlayerState::GetLives()
{
    return m_currentLives;
}

float PlayerState::GetPlayerSpeed()
{
    return m_playerSpeed;
}

void PlayerState::SetCurrentEffect(int effect)
{
    m_currentEffect = effect;
    SetPlayerSpeed();
}

int PlayerState::CurrentEffect()
{
    return m_currentEffect;
}

void PlayerState::AddLife()
{
    m_currentLives++;

    if (m_currentLives >= m_maxLives) {
        m_currentLives = 3;
    }
}

void PlayerState::AddHealth()
{
    m_currentHealth += 50.0f;

    if (m_currentHealth >= m_maxHealth) {
        m_currentHealth = 100.0f;
    }
}

void PlayerState::LoseLife()
{
    m_currentLives--;

    m_currentHealth = m_maxHealth;

    if (m_currentLives <= 0) {
        m_playerAlive = false;
    }
}

void PlayerState::TakeDamage(float damage)
{
    m_currentHealth -= damage;

    if (m_currentHealth <= 0.0f) {
        LoseLife();
    }
}

bool PlayerState::IsPlayerAlive()
{
    return m_playerAlive;
}

void PlayerState::Reset()
{
    m_currentHealth = m_maxHealth;
    m_currentLives = m_maxLives;

    m_playerAlive = true;
}

void PlayerState::SetPlayerSpeed()
{
    switch (m_currentEffect) {
    case 0:
        //default or normal effect
        m_playerSpeed = 80.0f;
        break;

    case 1:
        //slow time
        m_playerSpeed = 40.0f;
        break;

    case 2:
        //reverse
        m_playerSpeed = -80.0f;
        break;

    case 3:
        //fast time
        m_playerSpeed = 280.0f;
        break;

    default:
        m_playerSpeed = 80.0f;
        break;
    }
}

PlayerState::PlayerState()
{
}

PlayerState::~PlayerState()
{
    DestroyInstance();
}
