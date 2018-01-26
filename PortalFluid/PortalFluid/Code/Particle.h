#pragma once
#include <glm\vec3.hpp>
#include <vector>
#include <random>
#include <glm\detail\func_geometric.hpp>
#include <glm\gtc\constants.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <glm\gtx\vector_angle.hpp>
#include <glm\gtx\transform.hpp>

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
	ParticleEmitter(const size_t &_maxParticles, const glm::vec3 &_position, const glm::vec3 &_direction, const glm::vec3 &_gravity, const float &_cutoffAngle, const float &_speedMult)
		: maxParticles(_maxParticles), position(_position), gravity(_gravity), cutoffAngle(_cutoffAngle), phiDistribution(-glm::pi<double>(), glm::pi<double>()), thetaDistribution(0, _cutoffAngle), speedDistribution(10.0f, 15.0f), speedMult(_speedMult)
	{
		base = glm::lookAt(position, position + _direction, glm::vec3(0.0, 1.0, 0.0));
	}
	void update(const double &_currentTime, const double &_deltaTime);
	std::vector<Particle *> &getParticles();

private:
	std::default_random_engine randomEngine;
	std::uniform_real_distribution<> phiDistribution;
	std::uniform_real_distribution<> thetaDistribution;
	std::uniform_real_distribution<> speedDistribution;
	std::uniform_real_distribution<> particleEmittanceDistribution = std::uniform_real_distribution<>(0.1f, 0.2f);
	std::vector<Particle *> particles;
	size_t maxParticles;
	glm::vec3 position;
	glm::mat4 base;
	glm::vec3 gravity;
	float cutoffAngle;
	float speedMult;
	double lastEmittedParticleTime = 0;

	Particle *generateParticle();
};