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
	glm::quat tmp = glm::quat(glm::vec3(_pitchYawRoll.x, 0.0, _pitchYawRoll.z));
	// we always rotate yaw around the world space up axis
	glm::quat tmp1 = glm::quat(glm::angleAxis(_pitchYawRoll.y, glm::vec3(0.0, 1.0, 0.0)));
	rotation = glm::normalize(tmp * rotation * tmp1);

	needToUpdateViewMatrix = true;
}

void Camera::translate(const glm::vec3 &_translation)
{
	// extract forward and strafe directions from view matrix
	glm::vec3 forward(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]);
	glm::vec3 strafe(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);

	position += (_translation.z * forward + _translation.x * strafe);
	needToUpdateViewMatrix = true;
}

const glm::mat4 &Camera::getViewMatrix() const
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

glm::vec3 Camera::getForwardDirection() const
{
	glm::vec3 dir(glm::normalize(getViewMatrix()[2]));
	dir.z = -dir.z;
	return dir;
}

glm::vec3 Camera::getUpDirection() const
{
	return glm::normalize(getViewMatrix()[1]);;
}

void Camera::updateViewMatrix() const
{
	glm::mat4 translate;
	viewMatrix = glm::mat4_cast(rotation) * glm::translate(translate, -position);
}