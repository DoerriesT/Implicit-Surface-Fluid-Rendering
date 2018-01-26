#include "Particle.h"
#include <algorithm>
#include <iostream>
#include <glm\gtx\vector_angle.hpp>

void ParticleEmitter::update(const double &_currentTime, const double &_deltaTime)
{
	std::vector<Particle *> particlesToRemove;
	for (auto iter = particles.begin(); iter != particles.end(); ++iter)
	{
		Particle *particle = *iter;
		particle->speed += (float)_deltaTime * (gravity * speedMult);
		particle->position += (float)_deltaTime * particle->speed;
		
		if (particle->position.y < 0.0f)
		{
			particle->speed = glm::vec3(0.0);
			//particlesToRemove.push_back(particle);
		}
	}

	for (Particle *particle : particlesToRemove)
	{
		particles.erase(std::remove(particles.begin(), particles.end(), particle));
		particle->speed = glm::vec3(0.0f);
		delete particle;
	}

	if ((_currentTime - lastEmittedParticleTime >= particleEmittanceDistribution(randomEngine)) && particles.size() < maxParticles)
	{
		particles.push_back(generateParticle());
		lastEmittedParticleTime = _currentTime;
	}
}

std::vector<Particle *> &ParticleEmitter::getParticles()
{
	return particles;
}

Particle *ParticleEmitter::generateParticle()
{
	assert(particles.size() < maxParticles);

	float phi = phiDistribution(randomEngine);
	float theta = thetaDistribution(randomEngine);
	float speed = speedDistribution(randomEngine);
	
	float x = sin(theta) * cos(phi);
	float y = sin(theta) * sin(phi);
	float z = cos(theta);

	glm::vec3 particleSpeed = glm::vec3(x, y, z) * speed * speedMult;
	particleSpeed = base * glm::vec4(particleSpeed, 0.0);

	std::cout << "emitting particle" << std::endl;
	return new Particle(position, particleSpeed);
}
