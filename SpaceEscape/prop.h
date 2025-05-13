#ifndef _PROP_H__
#define _PROP_H__

#include "vector2.h"
#include "gameobject.h"
#include <string>

using namespace std;

class Renderer;
class Sprite;

class Prop : public GameObject {
public:
	Prop();
	~Prop();

	bool initialise(Renderer& renderer, string levelType);
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
	string GetFilePath(int num, std::string leveltype);

private:
	Prop(const Prop& prop);
	Prop& operator=(const Prop& prop);

public:

protected:
	Vector2 m_position;
	Sprite* m_sprite;

	bool m_alive;

private:

};

#endif // !_PROP_H__
