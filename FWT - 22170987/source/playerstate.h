#ifndef _PLAYERSTATE_H__
#define _PLAYERSTATE_H__

class PlayerState {
public:
	static PlayerState& GetInstance();
	static void DestroyInstance();
	void DebugDraw();

	float GetHealth();
	float GetMaxHealth();
	int GetLives();

	void AddLife();
	void LoseLife();
	void TakeDamage(float damage);

	bool IsPlayerAlive();

protected:

private:
	PlayerState();
	~PlayerState();
	PlayerState(const PlayerState& playerState);
	PlayerState& operator=(const PlayerState& playerState);

public:

protected:
	static PlayerState* sm_pInstance;

	float m_maxHealth = 100.0f;
	float m_currentHealth = 100.0f;
	int m_maxLives = 3;
	int m_currentLives = 3;

	bool m_playerAlive = true;

private:
};

#endif // !_PLAYERSTATE_H__
