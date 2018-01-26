#pragma once
#include <glm\vec3.hpp>
#include <vector>
#include <random>
#include <glm\detail\func_geometric.hpp>
#include <glm\gtc\constants.hpp>

struct Particle
{
	Particle(const glm::vec3 &_position, const glm::vec3 &_speed)
		:position(_position), speed(_speed) { }
	glm::vec3 position;
	glm::vec3 speed;
};

class ParticleEmitter
{
public:
	ParticleEmitter(const size_t &_maxParticles, const glm::vec3 &_position, const glm::vec3 &_direction, const glm::vec3 &_gravity, const float &_cutoffAngle)
		: maxParticles(_maxParticles), position(_position), direction(glm::normalize(_direction)), gravity(_gravity), cutoffAngle(_cutoffAngle), phiDistribution(0, _cutoffAngle), thetaDistribution(-glm::pi<double>(), glm::pi<double>()), speedDistribution(0.1f, 0.2f) { }
	void update(const double &_currentTime, const double &_deltaTime);
	std::vector<Particle> &getParticles();

private:
	std::default_random_engine randomEngine;
	std::uniform_real_distribution<> phiDistribution;
	std::uniform_real_distribution<> thetaDistribution;
	std::uniform_real_distribution<> speedDistribution;
	std::uniform_real_distribution<> particleEmittanceDistribution = std::uniform_real_distribution<>(0.1f, 1.0f);
	std::vector<Particle> particles;
	size_t maxParticles;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 gravity;
	float cutoffAngle;
	double lastEmittedParticleTime = 0;

	Particle generateParticle();
};