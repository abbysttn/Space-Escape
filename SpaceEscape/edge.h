#ifndef _EDGE_H__
#define _EDGE_H__

#include "vector2.h"
#include "gameobject.h"

using namespace std;

class Renderer;
class Sprite;

class Edge : public GameObject {
public:
	Edge();
	~Edge();

	bool initialise(Renderer& renderer, const char* filename);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	Vector2& Position();
	int GetSpriteWidth() const override;
	void SetColour(float red, float green, float blue);
	GameObject* Create() const override;
	bool isActive() const override;
	void reset() override;
	void SetActive(bool active);
	void SetRotation(float angle);

protected:

private:
	Edge(const Edge& edge);
	Edge& operator=(const Edge& edge);

public:

protected:
	Vector2 m_position;
	Sprite* m_sprite;

	bool m_alive;

private:

};

#endif // !_EDGE_H__
