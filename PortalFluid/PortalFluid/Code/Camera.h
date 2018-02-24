#pragma once
#include <glm\vec3.hpp>
#include <glm\mat4x4.hpp>
#include <glm\gtc\quaternion.hpp>

class Camera
{
public:
	/*
	 * Default constructs a Camera instance
	 */
	explicit Camera() = default;
	
	/*
	 * Constructs a Camera from a given position and rotation
	 */
	explicit Camera(const glm::vec3 &_position, const glm::quat &_rotation);

	/*
	 * Constructs a Camera from a given position and pitch, yaw and roll values
	 */
	explicit Camera(const glm::vec3 &_position, const glm::vec3 &_pitchYawRoll);

	/*
	 * Sets the camera rotation to the given rotation quaternion
	 */
	void setRotation(const glm::quat &_rotation);

	/*
	 * Sets the camera position to the given position vector
	 */
	void setPosition(const glm::vec3 &_position);

	/*
	 * Rotates the Camera by the given pitch, yaw and roll offsets
	 */
	void rotate(const glm::vec3 &_pitchYawRollOffset);

	/*
	 * Translates the Camera by the given offsets
	 */
	void translate(const glm::vec3 &_translationOffset);

	/*
	 * Returns a reference to the view matrix
	 */
	const glm::mat4 &getViewMatrix() const;

	/*
	 * Returns a reference to the position
	 */
	const glm::vec3 &getPosition() const;

	/*
	 * Returns a reference to the rotation
	 */
	const glm::quat &getRotation() const;

	/*
	 * Returns the camera forward direction
	 */
	glm::vec3 getForwardDirection() const;

	/*
	* Returns the camera up direction
	*/
	glm::vec3 getUpDirection() const;

private:
	glm::vec3 position;
	glm::quat rotation;
	// the following two members are mutable to allow users of the class to call getViewMatrix()
	// on const instances of Camera. getViewMatrix() may update the view matrix if necessary.
	mutable glm::mat4 viewMatrix;
	mutable bool needToUpdateViewMatrix = true;

	void updateViewMatrix() const;
};
