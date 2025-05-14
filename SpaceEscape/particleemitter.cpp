#include "particleemitter.h"
#include "particle.h"
#include "Renderer.h"
#include "sprite.h"

#include "inlinehelpers.h"
#include "imgui/imgui.h"

ParticleEmitter::ParticleEmitter() : m_pSharedSprite(0), m_fTimeElapsed(0.0f),
m_iSpawnBatchSize(5), m_fEmitRate(0.1f), m_fMaxLifespan(2.0f), m_fAccelerationScalar(100.0f),
 m_fMinAngle(0.0f), m_fMaxAngle(360.0f), m_fX(400.0f), m_fY(300.0f), m_active(true), m_scale(3.0f)
{
	m_fColour[0] = 1.0f;
	m_fColour[1] = 1.0f;
	m_fColour[2] = 1.0f;
}

ParticleEmitter::~ParticleEmitter()
{
	for (Particle* particle : m_particles) {
		delete particle;
		particle = 0;
	}

	delete m_pSharedSprite;
	m_pSharedSprite = 0;

}

bool ParticleEmitter::Initialise(Renderer& renderer, const char* filename)
{
	m_pSharedSprite = renderer.CreateSprite(filename);
	return (m_pSharedSprite != nullptr);
}

void ParticleEmitter::Process(float deltaTime)
{
	if (m_active) {
		m_fTimeElapsed += deltaTime;

		if (m_fTimeElapsed >= m_fEmitRate) {
			for (int i = 0; i < m_iSpawnBatchSize; i++) {
				Spawn();
			}

			m_fTimeElapsed = 0.0f;
		}

		for (size_t i = 0; i < m_particles.size(); i++) {
			m_particles[i]->Process(deltaTime);

			if (!m_particles[i]->m_bAlive) {
				delete m_particles[i];
				m_particles.erase(m_particles.begin() + i);
			}
		}
	}
}

void ParticleEmitter::Draw(Renderer& renderer)
{
	if (m_active) {
		for (Particle* particle : m_particles) {
			particle->Draw(renderer);
		}
	}
}

void ParticleEmitter::Spawn()
{
	Particle* particle = new Particle();
	particle->Initialise(*m_pSharedSprite);

	particle->SetScale(m_scale);

	particle->m_bAlive = true;
	particle->m_fCurrentAge = 0.0f;
	particle->m_fMaxLifespan = m_fMaxLifespan;

	particle->m_postion.x = m_fX;
	particle->m_postion.y = m_fY;

	particle->m_fColour[0] = m_fColour[0];
	particle->m_fColour[1] = m_fColour[1];
	particle->m_fColour[2] = m_fColour[2];

	float angle = (float)GetRandom((int)m_fMinAngle, (int)m_fMaxAngle);
	float radians = angle * 3.14159f / 180.0f;

	float speed = m_fAccelerationScalar;

	particle->m_velocity.x = cosf(radians) * speed;
	particle->m_velocity.y = sinf(radians) * speed;

	particle->m_acceleration.x = 0.0f;
	particle->m_acceleration.y = 98.1f;

	m_particles.push_back(particle);
}

void ParticleEmitter::DebugDraw()
{
	ImGui::Begin("Particle Emitter Debug");

	ImGui::SliderFloat("Position X", &m_fX, 0.0f, 800.0f);
	ImGui::SliderFloat("Position Y", &m_fY, 0.0f, 600.0f);
	ImGui::SliderFloat("Emit Rate", &m_fEmitRate, 0.01f, 1.0f);
	ImGui::SliderInt("Batch Size", &m_iSpawnBatchSize, 1, 20);
	ImGui::SliderFloat("Max Lifespan", &m_fMaxLifespan, 0.1f, 5.0f);
	ImGui::SliderFloat("Acceleration", &m_fAccelerationScalar, 0.0f, 500.0f);
	ImGui::SliderFloat("Min Angle", &m_fMinAngle, 0.0f, 360.0f);
	ImGui::SliderFloat("Max Angle", &m_fMaxAngle, 0.0f, 360.0f);
	ImGui::ColorEdit3("Particle Color", m_fColour);
	ImGui::Text("Active Particles: %d", static_cast<int>(m_particles.size()));

	ImGui::End();
}

void ParticleEmitter::SetAngles(float maxAngle, float minAngle)
{
	m_fMaxAngle = maxAngle;
	m_fMinAngle = minAngle;
}

void ParticleEmitter::SetColour(float colour[3])
{
	m_fColour[0] = colour[0];
	m_fColour[1] = colour[1];
	m_fColour[2] = colour[2];
}

void ParticleEmitter::SetEmitRate(float rate)
{
	m_fEmitRate = rate;
}

void ParticleEmitter::SetBatchSize(int size)
{
	m_iSpawnBatchSize = size;
}

void ParticleEmitter::SetLifeSpan(float life)
{
	m_fMaxLifespan = life;
}

void ParticleEmitter::SetPosition(float fX, float fY)
{
	m_fX = fX;
	m_fY = fY;
}

void ParticleEmitter::SetAcceleration(float rate)
{
	m_fAccelerationScalar = rate;
}

void ParticleEmitter::SetScale(float scale)
{
	m_scale = scale;

	for (Particle* particle : m_particles) {
		particle->SetScale(scale);
	}
}

void ParticleEmitter::SetActive(bool active)
{
	m_active = active;
}

void ParticleEmitter::Reset()
{
	m_particles.clear();
}
