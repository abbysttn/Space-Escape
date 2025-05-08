#ifndef _PLAYER_H__
#define _PLAYER_H__

#include "vector2.h"
#include "gameobject.h"

using namespace std;

class Renderer;
class AnimatedSprite;

class Player : public GameObject {
public:
	Player();
	~Player();

	bool Initialise(Renderer& renderer, const char* filename);
	void Process(float deltaTime);
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

	void ApplyPushBack(Vector2 direction);
	bool IsPushedBack();
	bool IsPushBackComplete();

	Vector2 GetUpdatedPushPosition(float deltaTime);

	void SetPushedBack(bool pushed);

	void AddDamage(float damage);

	
protected:
	void UpdatePushBack(float deltaTime);

private:
	Player(const Player& player);
	Player& operator=(const Player& player);

public:

protected:
	Vector2 m_position;
	AnimatedSprite* m_sprite;

	Vector2 m_pushbackVelocity;
	bool m_isPushed;
	bool m_pushbackComplete;

	bool m_alive;

	float m_health;
	int m_lives;

private:
};

#endif // !_PLAYER_H__
