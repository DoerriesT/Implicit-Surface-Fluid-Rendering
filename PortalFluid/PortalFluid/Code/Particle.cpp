#include "Particle.h"
#include <glm\gtx\vector_angle.hpp>
#include <glm\detail\func_geometric.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <glm\gtx\transform.hpp>

ParticleEmitter::ParticleEmitter(const size_t &_maxParticles, const glm::vec3 &_position, const glm::vec3 &_direction, const glm::vec3 &_gravity, const float &_cutoffAngle, const float &_speedMult)
	: thetaDistribution(0.0, _cutoffAngle),
	maxParticles(_maxParticles),
	position(_position),
	base(glm::lookAt(position, position + _direction, glm::vec3(0.0, 1.0, 0.0))),
	gravity(_gravity),
	cutoffAngle(_cutoffAngle),
	speedMult(_speedMult)
{
}

ParticleEmitter::~ParticleEmitter()
{
	for (Particle *particle : particles)
	{
		delete particle;
	}
}

void ParticleEmitter::update(const double &_currentTime, const double &_deltaTime)
{
	// update simulation
	for (auto iter = particles.begin(); iter != particles.end();)
	{
		Particle *particle = *iter;
		particle->speed += (float)_deltaTime * (gravity * speedMult);
		particle->position += (float)_deltaTime * particle->speed;
		
		// remove particles with y < 0.0
		if (particle->position.y < 0.0f)
		{
			iter = particles.erase(iter);
			delete particle;
		}
		else
		{
			++iter;
		}
	}

	// if sufficient time has passed since the last emitted particle and we are not at the maximum particle cap, emit a new particle
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
	// make sure we are not by mistake trying to create more particles than allowed
	assert(particles.size() < maxParticles);

	// generate random phi and theta spherical coordinate values 
	float phi = static_cast<float>(phiDistribution(randomEngine));
	float theta = static_cast<float>(thetaDistribution(randomEngine));
	// generate random speed value
	float speed = static_cast<float>(speedDistribution(randomEngine));
	
	// transform to cartesian coordinates
	float x = sin(theta) * cos(phi);
	float y = sin(theta) * sin(phi);
	float z = cos(theta);

	// create speed vector from coordinates on unit sphere, random speed and user specified speed multiplier
	glm::vec3 particleSpeed = glm::vec3(x, y, z) * speed * speedMult;
	// align particle speed (which also serves as direction) with emitter direction
	particleSpeed = base * glm::vec4(particleSpeed, 0.0);

	return new Particle(position, particleSpeed);
}
