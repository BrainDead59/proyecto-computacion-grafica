#pragma once

#include <glew.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>

#include <glfw3.h>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startMoveSpeed, GLfloat startTurnSpeed);

	void keyControl(bool* keys, GLfloat deltaTime, bool mode);
	void mouseControl(GLfloat xChange);

	glm::vec3 getCameraPosition();
	glm::vec3 getCameraDirection();
	glm::mat4 calculateViewMatrix();

	~Camera();

private:
	glm::vec3 positionTercera;
	glm::vec3 positionIsometrica;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	GLfloat yaw;
	GLfloat pitch=-30.0f;
	GLfloat distanciaCamara=180;
	GLint estado;

	GLfloat moveSpeed;
	GLfloat turnSpeed;

	void update();
};

