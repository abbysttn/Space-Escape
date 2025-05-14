#include "weaponstate.h"

WeaponState* WeaponState::sm_wInstance = 0;

WeaponState& WeaponState::GetInstance()
{
    if (sm_wInstance == 0) {
        sm_wInstance = new WeaponState();
    }

    return (*sm_wInstance);
}

void WeaponState::DestroyInstance()
{
    delete sm_wInstance;
    sm_wInstance = 0;
}

void WeaponState::DebugDraw()
{
}

void WeaponState::CreateWeaponMap()
{
    for (int i = 0; i < 4; i++) {
        if (m_weaponsObtained.find(i) == m_weaponsObtained.end()) {
            m_weaponsObtained.try_emplace(i, false);
        }
    }

    auto initialWeapon = m_weaponsObtained.find(0);

    if (initialWeapon != m_weaponsObtained.end()) {
        initialWeapon->second = true;
        m_currentWeapon = 0;
    }
}

void WeaponState::SetCurrentWeapon(int weapon)
{
    m_currentWeapon = weapon;

    auto setWeapon = m_weaponsObtained.find(weapon);

    if (setWeapon != m_weaponsObtained.end() && setWeapon->second == false) {
        setWeapon->second = true;
    }
}

int WeaponState::GetNextWeapon()
{
    return m_nextWeapon;
}

int WeaponState::GetCurrentWeapon()
{
    return m_currentWeapon;
}

void WeaponState::AddWeapon(int weapon)
{
    if (m_weaponsObtained.find(weapon) == m_weaponsObtained.end()) return;

    m_weaponsObtained.find(weapon)->second = true;
    m_currentWeapon = weapon;
}

int WeaponState::GetWeaponUpgrade()
{
    for (int i = 0; i < 4; i++) {
        auto weapon = m_weaponsObtained.find(i);

        if (weapon == m_weaponsObtained.end()) return NULL;

        if (weapon->second == false) {
            return i;
            m_nextWeapon = i;
        }
    }

    return NULL;
}

void WeaponState::Reset()
{
    m_weaponsObtained.clear();
}

WeaponState::WeaponState()
{
}

WeaponState::~WeaponState()
{
    DestroyInstance();
}
