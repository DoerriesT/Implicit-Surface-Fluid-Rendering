#pragma once
#include <glm\vec3.hpp>
#include <glm\mat4x4.hpp>
#include <glm\gtc\quaternion.hpp>

class Camera
{
public:
	Camera() = default;
	explicit Camera(const glm::vec3 &_position, const glm::quat &_rotation);
	explicit Camera(const glm::vec3 &_position, const glm::vec3 &_pitchYawRoll);
	void setRotation(const glm::quat &_rotation);
	void setPosition(const glm::vec3 &_position);
	void rotate(const glm::vec3 &_pitchYawRollOffset);
	void translate(const glm::vec3 &_translationOffset);
	const glm::mat4 &getViewMatrix();
	const glm::vec3 &getPosition() const;
	const glm::quat &getRotation() const;
	glm::vec3 getForwardDirection();
	glm::vec3 getUpDirection();

private:
	glm::vec3 position;
	glm::quat rotation;
	glm::mat4 viewMatrix;
	bool needToUpdateViewMatrix = true;

	void updateViewMatrix();
};
