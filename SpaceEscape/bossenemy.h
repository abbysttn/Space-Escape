#ifndef _BOSSENEMY_H__
#define _BOSSENEMY_H__

#include "vector2.h"
#include "gameobject.h"
#include <string>

using namespace std;

class Renderer;
class AnimatedSprite;

class BossEnemy : public GameObject {
public:
	BossEnemy();
	~BossEnemy();

	bool Initialise(Renderer& renderer, int levelNum);
	void Process(float deltaTime, Vector2 playerPos);
	void Draw(Renderer& renderer);
	Vector2& Position();
	int GetSpriteWidth() const override;
	void SetActive(bool active);

	GameObject* Create() const override;
	bool isActive() const override;
	void reset() override;

	void SetFrameTime(float time);
	void SetLoop(bool loop);
	void SetColour(float red, float green, float blue);

	Vector2 GetUpdatedPosition(float deltaTime, Vector2 playerPos);

	void ResetWander(Vector2 direction);
	void ApplyPushBack(Vector2 direction);

	void AddDamage(float weaponDamage);

	float GetDamageDealt();
	void SetAttackDamage(int levelNum, char difficulty);

	bool DropsItems();
	int GetDroppedItem();

	bool CanSeePlayer();
	void SetGameDifficulty(char difficulty);

protected:
	void UpdatePushBack(float deltaTime);
	void GetBoss(int levelNum);

	void InitBoss();
	void InitFinalBoss();

private:
	BossEnemy(const BossEnemy& enemy);
	BossEnemy& operator=(const BossEnemy& enemy);

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
	float m_attackDamage;
	char m_gameDifficulty;

	bool m_dropsParts;

	float m_bossColour[3];

private:
};

#endif // !_BOSSENEMY_H__
