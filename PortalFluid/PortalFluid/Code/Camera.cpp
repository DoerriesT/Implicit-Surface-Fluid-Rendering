#include <glm\gtc\matrix_transform.hpp>
#include "Camera.h"

Camera::Camera(const glm::vec3 &_position, const glm::quat &_rotation)
	:position(_position), rotation(_rotation)
{
}

Camera::Camera(const glm::vec3 & _position, const glm::vec3 &_pitchYawRoll)
	: position(_position), rotation(_pitchYawRoll)
{
}

void Camera::setRotation(const glm::quat &_rotation)
{
	rotation = _rotation;
	needToUpdateViewMatrix = true;
}

void Camera::setPosition(const glm::vec3 &_position)
{
	position = _position;
	needToUpdateViewMatrix = true;
}

void Camera::rotate(const glm::vec3 &_pitchYawRoll)
{
	glm::quat tmp = glm::quat(glm::vec3(_pitchYawRoll.x, 0.0, 0.0));
	glm::quat tmp1 = glm::quat(glm::angleAxis(_pitchYawRoll.y, glm::vec3(0.0, 1.0, 0.0)));
	rotation = glm::normalize(tmp * rotation * tmp1);

	needToUpdateViewMatrix = true;
}

void Camera::translate(const glm::vec3 &_translation)
{
	glm::vec3 forward(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]);
	glm::vec3 strafe(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);

	static const float speed = 1.12f;

	position += (_translation.z * forward + _translation.x * strafe) * speed;
	needToUpdateViewMatrix = true;
}

const glm::mat4 &Camera::getViewMatrix()
{
	if (needToUpdateViewMatrix)
	{
		updateViewMatrix();
		needToUpdateViewMatrix = false;
	}
	return viewMatrix;
}

const glm::vec3 &Camera::getPosition() const
{
	return position;
}

const glm::quat & Camera::getRotation() const
{
	return rotation;
}

glm::vec3 Camera::getForwardDirection()
{
	glm::vec3 dir(glm::normalize(getViewMatrix()[2]));
	dir.z = -dir.z;
	return dir;
}

glm::vec3 Camera::getUpDirection()
{
	return glm::normalize(getViewMatrix()[1]);;
}

void Camera::updateViewMatrix()
{
	glm::mat4 translate;
	viewMatrix = glm::mat4_cast(rotation) * glm::translate(translate, -position);
}