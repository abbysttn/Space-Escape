#include "playerstate.h"
#include "logmanager.h"

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

void PlayerState::AddLife()
{
    m_currentLives++;
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

PlayerState::PlayerState()
{
}

PlayerState::~PlayerState()
{
    DestroyInstance();
}
