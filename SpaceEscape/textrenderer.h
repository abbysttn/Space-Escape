#ifndef _TEXTRENDERER_H__
#define _TEXTRENDERER_H__

#include "gameobject.h"
#include "vector2.h"
using namespace std;

class Renderer;
class Sprite;

class TextRenderer : public GameObject {
public:
	TextRenderer();
	~TextRenderer();

	bool initialise(Renderer& renderer, const char* text, int pointSize);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	Vector2& Position();
	int GetSpriteWidth() const override;
	int GetSpriteHeight();
	void SetColour(float red, float green, float blue);
	GameObject* Create() const override;
	bool isActive() const override;
	void reset() override;

protected:

private:
	TextRenderer(const TextRenderer& textRenderer);
	TextRenderer& operator=(const TextRenderer& textRenderer);

public:

protected:
	Vector2 m_position;
	Sprite* m_sprite;

private:
};

#endif // !_TEXTRENDERER_H__
