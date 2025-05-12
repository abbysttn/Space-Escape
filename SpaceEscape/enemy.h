#ifndef _ENEMY_H__
#define _ENEMY_H__

#include "vector2.h"
#include "gameobject.h"

using namespace std;

class Renderer;
class AnimatedSprite;

class Enemy : public GameObject {
public:
	Enemy();
	~Enemy();

	bool Initialise(Renderer& renderer, const char* filename);
	void Process(float deltaTime, Vector2 playerPos);
	void Draw(Renderer& renderer);
	Vector2& Position();
	int GetSpriteWidth();
	void SetColour(float red, float green, float blue);
	void SetActive(bool active);
	void SetRotation(float angle);

	GameObject* Create() const override;
	bool isActive() const override;
	void reset() override;

	void SetFrameTime(float time);
	void SetLoop(bool loop);

	bool IsRunning();
	void SetRunning();

	Vector2 GetUpdatedPosition(float deltaTime, Vector2 playerPos);

	void ResetWander(Vector2 direction);
	void ApplyPushBack(Vector2 direction);

	void AddDamage(float weaponDamage);
	void SetEnemyType(char type);

	float GetDamageDealt();
	void SetAttackDamage(char type, char difficulty);

	void SetEnemyDrop(bool drop);

protected:
	void UpdatePushBack(float deltaTime);
	

private:
	Enemy(const Enemy& enemy);
	Enemy& operator=(const Enemy& enemy);

public:

protected:
	Vector2 m_position;
	Vector2 m_velocity;
	Vector2 m_wanderDirection;

	Vector2 m_pushbackVelocity;
	bool m_isPushed;

	float m_speed;
	float m_wanderTime;
	float m_wanderDuration;

	AnimatedSprite* m_sprite;

	bool m_alive;

	bool m_canSeePlayer;

	float m_health;
	char m_enemyType;
	float m_attackDamage;
	char m_gameDifficulty;

	bool m_dropsParts;

private:
};

#endif // !_ENEMY_H__
