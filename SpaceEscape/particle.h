#ifndef PARTICLE_H__
#define PARTICLE_H__
// Local includes:
#include "vector2.h"
// Forward declarations:
class Renderer;
class Sprite;
// Class declaration:
class Particle
{
	// Member methods:
public:
	Particle();
	~Particle();
	bool Initialise(Sprite& sprite);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
protected:
private:
	Particle(const Particle& particle);
	Particle& operator=(const Particle& particle);
	// Member data:
public:
	Sprite* m_pSharedSprite;
	Vector2 m_postion;
	Vector2 m_velocity;
	Vector2 m_acceleration;
	float m_fMaxLifespan;
	float m_fCurrentAge;
	float m_fColour[3];
	bool m_bAlive;
protected:
private:
};
#endif // PARTICLE_H__
