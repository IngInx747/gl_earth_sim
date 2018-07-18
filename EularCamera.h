#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement.
// Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement_Type {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {
public:

	// Default camera values
	static float YAW;
	static float PITCH;
	static float SPEED;
	static float HIGH_SPEED;
	static float SENSITIVITY;
	static float FOV;

	// Camera attribute
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 worldUp;
	glm::vec3 right;

	// Eular angles
	float yaw;
	float pitch;

	// Camera options
	float fov;
	float speed;
	float sensitivity;

	Camera( // Init with vector
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw   = YAW,
		float pitch = PITCH);

	Camera( // Init with scalar values
		float posX, float posY, float posZ,
		float upX,  float upY,  float upZ,
		float yaw,  float pitch);

	// Get view matrix based on Eular angles
	glm::mat4 getViewMatrix();

	// Process keyboard events
	void processKeyboard(Camera_Movement_Type move_type, float deltaTime);

	// Process mouse movement events
	void processMouse(float offsetX, float offsetY, GLboolean constrainPitch = true);

	// Process mouse scroller events
	void processScroll(float offsetY);

	// Adjust moving speed
	void processAccerlate(bool accer = false);

private:

	// Update vectors of the camera
	void update();
};

#endif