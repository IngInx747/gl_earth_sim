#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <EularCamera.h>

float Camera :: YAW         = -90.0f;
float Camera :: PITCH       =  0.0f;
float Camera :: SPEED       =  2.5f;
float Camera :: HIGH_SPEED  =  10.0f;
float Camera :: SENSITIVITY =  0.1f;
float Camera :: FOV         =  45.0f;

Camera :: Camera( // Init with vector
		glm::vec3 position,
		glm::vec3 up,
		float yaw,
		float pitch) :
position(position), yaw(yaw), pitch(pitch) {
	//this->position = position;
	//this->yaw      = yaw;
	//this->pitch    = pitch;
	worldUp        = up;
	front          = glm::vec3(0.0f, 0.0f, -1.0f),
	speed          = SPEED;
	sensitivity    = SENSITIVITY;
	fov            = FOV;
	update();
}

Camera :: Camera( // Init with scalar values
		float posX, float posY, float posZ,
		float upX,  float upY,  float upZ,
		float yaw,  float pitch) : 
position(posX, posY, posZ), yaw(yaw), pitch(pitch) {
	//this->position = glm::vec3(posX, posY, posZ);
	//this->yaw      = yaw;
	//this->pitch    = pitch;
	worldUp        = glm::vec3(upX, upY, upZ);
	front          = glm::vec3(0.0f, 0.0f, -1.0f),
	speed          = SPEED;
	sensitivity    = SENSITIVITY;
	fov            = FOV;
	update();
}

// Get view matrix based on Eular angles
glm::mat4 Camera :: getViewMatrix() {
	// lookAt(position, target, up direction)
	return glm::lookAt(position, position + front, up);
}

// Process keyboard events
void Camera :: processKeyboard(Camera_Movement_Type move_type, float deltaTime) {
	float velocity = speed * deltaTime;
	if (move_type == FORWARD)  position += front   * velocity;
	if (move_type == BACKWARD) position -= front   * velocity;
	if (move_type == LEFT)     position -= right   * velocity;
	if (move_type == RIGHT)    position += right   * velocity;
	if (move_type == UP)       position += worldUp * velocity;
	if (move_type == DOWN)     position -= worldUp * velocity;
}

// Process mouse movement events
void Camera :: processMouse(float offsetX, float offsetY, GLboolean constrainPitch) {
	offsetX *= sensitivity;
	offsetY *= sensitivity;

	yaw   += offsetX;
	pitch += offsetY;

	if (constrainPitch) {
		if (pitch > 89.0f)  pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;
	}

	update();
}

// Process mouse scroller events
void Camera :: processScroll(float offsetY) {
	if (fov >= 1.0f && fov <= 150.0f) fov -= offsetY;
	if (fov < 1.0f) fov = 1.0f;
	if (fov > 150.0f) fov = 150.0f;
}

// Adjust moving speed
void Camera :: processAccerlate(bool accer) {
	if (accer) speed = HIGH_SPEED;
	else speed = SPEED;
}

// Update front vector of the camera
void Camera :: update() {
	glm::vec3 tmpFront;
	tmpFront.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	tmpFront.y = sin(glm::radians(pitch));
	tmpFront.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	front = glm::normalize(tmpFront);
	right = glm::normalize(glm::cross(front, worldUp));
	up    = glm::normalize(glm::cross(right, front));
}
