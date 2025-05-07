#ifndef _ENEMYSPAWNER_H__
#define _ENEMYSPAWNER_H__

#include "vector2.h"
#include "gameobject.h"
#include <string>

using namespace std;

class Renderer;
class Sprite;

class EnemySpawner : public GameObject {
public:
	EnemySpawner();
	~EnemySpawner();

	bool Initialise(Renderer& renderer);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	Vector2& Position();
	int GetSpriteWidth();
	GameObject* Create() const override;
	bool isActive() const override;
	void reset() override;
	void SetActive(bool active);

protected:

private:
	EnemySpawner(const EnemySpawner& enemySpawner);
	EnemySpawner& operator=(const EnemySpawner& enemySpawner);

public:

protected:
	Vector2 m_position;
	Sprite* m_sprite;

	bool m_alive;

private:

};

#endif // !_ENEMYSPAWNER_H__