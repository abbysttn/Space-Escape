#include "weapon.h"

#include "renderer.h"
#include "sprite.h"

#include <cassert>
#include <cstdlib>

Weapon::Weapon() : m_isLeft(false), m_sprite(0), m_isSwinging(false), m_swingSpeed(500.0f), m_swingAngle(0.0f), m_swingDirection(1.0f), m_maxRightSwingAngle(90.0f), m_maxLeftSwingAngle(90.0f), m_returnSpeed(300.0f) {}

Weapon::~Weapon()
{
	delete m_sprite;
	m_sprite = 0;
}

bool Weapon::initialise(Renderer& renderer, const char* filePath)
{
    m_sprite = renderer.CreateSprite(filePath);

    if (!m_sprite) {
        return false;
    }

    m_position.x = 0;
    m_position.y = 0;

    m_sprite->SetScale(1.0f);

    return true;
}

void Weapon::Process(float deltaTime)
{
    if (!m_sprite || !m_isWeapon) {
        return;
    }


    if (m_isSwinging && m_weaponType == 'M') {
        m_swingAngle += m_swingDirection * m_swingSpeed * deltaTime;

        if (m_isLeft) {
            if (m_swingAngle <= m_maxLeftSwingAngle) {
                m_swingDirection = 1.0f;
                m_swingSpeed = 800.0f;
            }

            if (m_swingAngle >= 180.0f) {
                m_swingAngle = 180.0f;
                m_swingSpeed = 500.0f;
                m_isSwinging = false;
                m_swingDirection = -1.0f;
            }
        }
        else {
            if (m_swingAngle >= m_maxRightSwingAngle) {
                m_swingDirection = -1.0f;
                m_swingSpeed = 800.0f;
            }

            if (m_swingAngle <= 0.0f) {
                m_swingAngle = 0.0f;
                m_swingSpeed = 500.0f;
                m_isSwinging = false;
                m_swingDirection = 1.0f;
            }
        }

        m_sprite->SetAngle(m_swingAngle);
    }

    m_sprite->SetX(static_cast<int>(m_position.x));
    m_sprite->SetY(static_cast<int>(m_position.y));

    m_sprite->Process(deltaTime);
}

void Weapon::Draw(Renderer& renderer)
{
    m_sprite->Draw(renderer);
}

Vector2& Weapon::Position()
{
    return m_position;
}

int Weapon::GetSpriteWidth()
{
    return m_sprite->GetWidth();
}

int Weapon::GetSpriteHeight()
{
    return m_sprite->GetHeight();
}

void Weapon::SetColour(float red, float green, float blue)
{
    m_sprite->SetRedTint(red);
    m_sprite->SetGreenTint(green);
    m_sprite->SetBlueTint(blue);
}

GameObject* Weapon::Create() const
{
    return new Weapon();
}

bool Weapon::isActive() const
{
    return m_isWeapon;
}

void Weapon::reset()
{
    m_position.x = 0;
    m_position.y = 0;
}

void Weapon::SetRotation(float angle)
{
    m_sprite->SetAngle(angle);
    m_isLeft = (angle > 0);
}

void Weapon::SetWeapon(bool isWeapon)
{
    m_isWeapon = isWeapon;
}

void Weapon::Swing()
{
    if (!m_isSwinging) {
        m_isSwinging = true;
        if (m_isLeft) {
            m_swingDirection = -1.0f;
            m_swingAngle = 180.0f;
        }
        else {
            m_swingDirection = 1.0f;
            m_swingAngle = 0.0f;
        }
        m_swingSpeed = 500.0f;
    }
}

void Weapon::SetWeaponType(char type)
{
    m_weaponType = type;
}

char Weapon::GetWeaponType()
{
    return m_weaponType;
}

void Weapon::SetSize(float size)
{
    m_sprite->SetScale(size);
}

void Weapon::SetOffset(float offset)
{
    weaponOffset = offset;
}

float Weapon::GetOffset()
{
    return weaponOffset;
}

bool Weapon::IsSwinging()
{
    return m_isSwinging;
}

void Weapon::SetWeaponDamage(int weaponNum)
{
    switch (weaponNum) {
    case 0:
        m_damage = 2.0f;
        break;

    case 1:
        m_damage = 10.0f;
        break;

    case 2:
        m_damage = 20.0f;
        break;

    case 3:
        m_damage = 30.0f;
        break;

    default:
        m_damage = 2.0f;
        break;
    }
}

float Weapon::GetWeaponDamage()
{
    return m_damage;
}
