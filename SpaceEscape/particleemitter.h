#ifndef PARTICLEEMITTER_H__
#define PARTICLEEMITTER_H__
// Library includes:
#include <vector>
#include <string>

using namespace std;
// Forward declarations:
class Renderer;
class Sprite;
class Particle;
// Class declaration:
class ParticleEmitter
{
	// Member methods:
public:
	ParticleEmitter();
	~ParticleEmitter();
	bool Initialise(Renderer& renderer, const char* filename);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	void Spawn();
	void DebugDraw();

	void SetAngles(float maxAngle, float minAngle);
	void SetColour(float colour[3]);
	void SetEmitRate(float rate);
	void SetBatchSize(int size);
	void SetLifeSpan(float life);
	void SetPosition(float fX, float fY);
	void SetAcceleration(float rate);

protected:
private:
	ParticleEmitter(const ParticleEmitter& particleemitter);
	ParticleEmitter& operator=(const ParticleEmitter& particleemitter);
	// Member data:
public:
protected:
	Sprite* m_pSharedSprite;
	std::vector<Particle*> m_particles;
	float m_fTimeElapsed;
	int m_iSpawnBatchSize;
	float m_fEmitRate;
	float m_fMaxLifespan;
	float m_fAccelerationScalar;
	float m_fColour[3];
	float m_fMinAngle;
	float m_fMaxAngle;
	float m_fX;
	float m_fY;

private:
};
#endif // PARTICLEEMITTER_H__