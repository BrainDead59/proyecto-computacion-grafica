#include "Camera.h"

Camera::Camera() {}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
	positionTercera = startPosition;
	positionIsometrica = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	front = glm::vec3(0.0f, 0.0f, -1.0f);
	estado = 0;

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime, bool mode)
{
	GLfloat velocity = moveSpeed * deltaTime;

	if (mode) {
		estado = 1;
		if (keys[GLFW_KEY_W])
		{
			positionTercera += front * velocity;
		}

		if (keys[GLFW_KEY_S])
		{
			positionTercera -= front * velocity;
		}

		if (keys[GLFW_KEY_A])
		{
			positionTercera -= right * velocity;
		}

		if (keys[GLFW_KEY_D])
		{
			positionTercera += right * velocity;
		}

		positionTercera.y = 30.0f;
	}
	else {
		estado = 0;
		if (keys[GLFW_KEY_W])
		{
			distanciaCamara -= 0.1f;
		}

		if (keys[GLFW_KEY_S])
		{
			distanciaCamara += 0.1f;
		}
	}
}

void Camera::mouseControl(GLfloat xChange)
{
	if (estado) {
		xChange *= turnSpeed;
		yaw += xChange;
		update();
	}
	else {
		update();
	}
}

glm::mat4 Camera::calculateViewMatrix()
{
	if (estado) {
		return glm::lookAt(positionTercera, positionTercera + front, up);
	}
	return glm::lookAt(positionIsometrica, glm::vec3(0.0,0.0,-1.0), glm::vec3(0.0, 1.0, 0.0));
}

glm::vec3 Camera::getCameraPosition()
{
	if (estado) {
		return positionTercera;
	}
	return positionIsometrica;
}

glm::vec3 Camera::getCameraDirection()
{
	return glm::normalize(front);
}

void Camera::update()
{
	if (estado) {
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(front);

		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
	}else {
		positionIsometrica.x = distanciaCamara * sin(glm::radians(pitch));
		positionIsometrica.y = distanciaCamara * -sin(glm::radians(pitch));
		positionIsometrica.z = distanciaCamara * cos(glm::radians(pitch));
	}
}

Camera::~Camera()
{
}
