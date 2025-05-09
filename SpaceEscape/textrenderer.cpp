#include "textrenderer.h"

#include "Renderer.h"
#include "sprite.h"

TextRenderer::TextRenderer() : m_sprite(0) {}

TextRenderer::~TextRenderer()
{
	delete m_sprite;
	m_sprite = 0;
}

bool TextRenderer::initialise(Renderer& renderer, const char* text, int pointSize)
{
	renderer.CreateStaticText(text, pointSize);
	m_sprite = renderer.CreateSprite(text);

	m_sprite->SetAlpha(1.0f);

	if (!m_sprite) return false;


}

void TextRenderer::Process(float deltaTime)
{
	m_sprite->SetX(static_cast<int>(m_position.x));
	m_sprite->SetY(static_cast<int>(m_position.y));
	m_sprite->Process(deltaTime);
}

void TextRenderer::Draw(Renderer& renderer)
{
	m_sprite->Draw(renderer);
}

Vector2& TextRenderer::Position()
{
	return m_position;
}

int TextRenderer::GetSpriteWidth()
{
	return m_sprite->GetWidth();
}

int TextRenderer::GetSpriteHeight()
{
	return m_sprite->GetHeight();
}

void TextRenderer::SetColour(float red, float green, float blue)
{
	m_sprite->SetRedTint(red);
	m_sprite->SetGreenTint(green);
	m_sprite->SetBlueTint(blue);
}

GameObject* TextRenderer::Create() const
{
	return new TextRenderer();
}

bool TextRenderer::isActive() const
{
	return true;
}

void TextRenderer::reset()
{
	m_position.x = 0;
	m_position.y = 0;
}
