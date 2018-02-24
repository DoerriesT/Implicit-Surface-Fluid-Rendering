#pragma once
#include <glm\vec3.hpp>
#include <glm\mat4x4.hpp>
#include <glm\gtc\constants.hpp>
#include <vector>
#include <random>

/*
 * Stores speed and position of a single particle.
 */
struct Particle
{
	explicit Particle(const glm::vec3 &_position, const glm::vec3 &_speed)
		:position(_position), 
		speed(_speed) 
	{ }
	glm::vec3 position;
	glm::vec3 speed;
};

/*
 * Emits and simulates a configurable amount of particles.
 */
class ParticleEmitter
{
public:
	/*
	 * Constructs a new ParticleEmitter with the following paramters:
	 * _maxParticles			maximum number of simulated particles
	 * _position				particle emitter position in world space
	 * _direction				general direction into which particles are emitted
	 * _gravity					vector specifying the direction and magnitude of gravity
	 * _cutoffAngle				maximum angle between emitter direction and particle direction
	 * _speedMult				speed multiplier to particle speeds
	 */
	explicit ParticleEmitter(const size_t &_maxParticles, const glm::vec3 &_position, const glm::vec3 &_direction, const glm::vec3 &_gravity, const float &_cutoffAngle, const float &_speedMult);

	/*
	 * Destructor
	 */
	~ParticleEmitter();
	
	/*
	 * Update particle simulation. Particles with a y value of less than 0.0 are removed.
	 * If the maximum number of particles is not currently reached and the last particle
	 * emittance is some random amount time ago, emit a new particle
	 */
	void update(const double &_currentTime, const double &_deltaTime);

	/*
	 * Returns a reference to the vector of simulated particles
	 */
	std::vector<Particle *> &getParticles();

private:
	// random engine to be used by the random distributions
	std::default_random_engine randomEngine;
	// distribution of values on a circle
	std::uniform_real_distribution<> phiDistribution = std::uniform_real_distribution<>(-glm::pi<double>(), glm::pi<double>());
	// distribution of values between 0 and a user defined cutoff angle
	std::uniform_real_distribution<> thetaDistribution;
	// distribution of different particle speeds
	std::uniform_real_distribution<> speedDistribution = std::uniform_real_distribution<>(10.0f, 15.0f);
	// distribution of different particle emittance times
	std::uniform_real_distribution<> particleEmittanceDistribution = std::uniform_real_distribution<>(0.1f, 0.2f);
	// vector of all active particles
	std::vector<Particle *> particles;
	// maximum number of particles
	std::size_t maxParticles;
	// particle emitter position
	glm::vec3 position;
	// matrix to align particle speed/direction with particle emitter direction
	glm::mat4 base;
	// gravity vector dragging particles into a direction
	glm::vec3 gravity;
	// maximum angle between emitter direction and particle direction
	float cutoffAngle;
	// speed multiplier for particles
	float speedMult;
	// remember when the last particle was emitted
	double lastEmittedParticleTime = 0;

	/*
	 * Returns a pointer to a new heap allocated particle. The caller is responsible for deleting.
	 */
	Particle *generateParticle();
};