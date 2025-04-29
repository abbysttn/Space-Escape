#include "particleemitter.h"
#include "particle.h"
#include "Renderer.h"
#include "sprite.h"

#include "inlinehelpers.h"
#include "imgui/imgui.h"

ParticleEmitter::ParticleEmitter() : m_pSharedSprite(0), m_fTimeElapsed(0.0f),
m_iSpawnBatchSize(5), m_fEmitRate(0.1f), m_fMaxLifespan(2.0f), m_fAccelerationScalar(100.0f),
 m_fMinAngle(0.0f), m_fMaxAngle(360.0f), m_fX(400.0f), m_fY(300.0f) 
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

bool ParticleEmitter::Initialise(Renderer& renderer)
{
	m_pSharedSprite = renderer.CreateSprite("..//assets//ball.png");
	return (m_pSharedSprite != nullptr);
}

void ParticleEmitter::Process(float deltaTime)
{
	m_fTimeElapsed += deltaTime;

	if (m_fTimeElapsed >= m_fEmitRate) {
		for (int i = 0; i < m_iSpawnBatchSize; i++) {
			Spawn();
		}

		m_fTimeElapsed = 0.0f;
	}

	for (int i = 0; i < m_particles.size(); i++) {
		m_particles[i]->Process(deltaTime);

		if (!m_particles[i]->m_bAlive) {
			delete m_particles[i];
			m_particles.erase(m_particles.begin() + i);
		}
	}
}

void ParticleEmitter::Draw(Renderer& renderer)
{
	for (Particle* particle : m_particles) {
		particle->Draw(renderer);
	}
}

void ParticleEmitter::Spawn()
{
	Particle* particle = new Particle();
	particle->Initialise(*m_pSharedSprite);

	particle->m_bAlive = true;
	particle->m_fCurrentAge = 0.0f;
	particle->m_fMaxLifespan = m_fMaxLifespan;

	particle->m_postion.x = m_fX;
	particle->m_postion.y = m_fY;

	particle->m_fColour[0] = m_fColour[0];
	particle->m_fColour[1] = m_fColour[1];
	particle->m_fColour[2] = m_fColour[2];

	float angle = GetRandom(m_fMinAngle, m_fMaxAngle);
	float radians = angle * 3.14159 / 180.0f;

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
