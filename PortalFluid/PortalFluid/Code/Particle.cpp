#include "Particle.h"
#include <algorithm>
#include <iostream>

void ParticleEmitter::update(const double &_currentTime, const double &_deltaTime)
{
	std::vector<Particle> survivingParticles;
	for (auto iter = particles.begin(); iter != particles.end(); ++iter)
	{
		Particle &particle = *iter;
		particle.speed += (float)_deltaTime * gravity;
		particle.position += (float)_deltaTime * particle.speed;
		std::cout << particle.position.y << std::endl;
		if (particle.position.y > 0.0f)
		{
			survivingParticles.push_back(particle);
		}
		else
		{
			std::cout << "deleting particle" << std::endl;
		}
	}
	particles = survivingParticles;

	if ((_currentTime - lastEmittedParticleTime <= particleEmittanceDistribution(randomEngine)) && particles.size() < maxParticles)
	{
		particles.push_back(generateParticle());
		lastEmittedParticleTime = _currentTime;
	}
	std::cout << particles.size() << std::endl;
}

std::vector<Particle>& ParticleEmitter::getParticles()
{
	return particles;
}

Particle ParticleEmitter::generateParticle()
{
	assert(particles.size() < maxParticles);

	float phi = phiDistribution(randomEngine);
	float theta = thetaDistribution(randomEngine);
	float speed = speedDistribution(randomEngine);

	//theta += glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), direction);
	
	float x = sin(theta) * cos(phi);
	float y = sin(theta) * sin(phi);
	float z = cos(theta);

	glm::vec3 particleSpeed = glm::vec3(x, y, z) * speed * 0.001f;

	std::cout << "emitting particle" << std::endl;
	return Particle(position, particleSpeed);
}
