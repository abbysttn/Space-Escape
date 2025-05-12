#ifndef _RIFTVIAL_H__
#define _RIFTVIAL_H__

#include "vector2.h"

using namespace std;

class Renderer;
class Sprite;

class RiftVial {
public:
	RiftVial();
	~RiftVial();

	bool Initialise(Renderer& renderer);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	Vector2& Position();
	int GetSpriteWidth();

	void SetRotation(float angle);
	void SetCollected(bool collected);
	bool IsCollected();

	void Drop(Vector2 position);

protected:

private:
	RiftVial(const RiftVial& riftVial);
	RiftVial& operator=(const RiftVial& riftVial);

public:

protected:
	Vector2 m_position;
	Sprite* m_sprite;

	bool m_isCollected;

private:
};

#endif // !_RIFTVIAL_H__
