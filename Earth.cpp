#include <iostream>
#include <sstream>
#include <string>
#include <memory>

/** Basic GLFW header */
//#include <GL/glew.h>	// Important - this header must come before glfw3 header
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/** GLFW Math */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/** GLFW Texture header */
#include <stb_image/stb_image.h> // Support several formats of image file

/** Shader Wrapper */
#include <ShaderProgram.h>

/** Camera Wrapper */
#include <EularCamera.h>

/** Model Wrapper */
#include <Model.h>
#include <Primitives.h>
#include <Skybox.h>
#include <ParallelShadow.h>

// Global Variables
const char* APP_TITLE = "Earth Sim";
const int gWindowWidth = 1280;
const int gWindowHeight = 720;
GLFWwindow* gWindow = NULL;

// Camera system
Camera camera(glm::vec3(0.0f, 0.0f, 30.0f));

// Shader control
bool enableTorch = true;
bool enableNormal = true;

// Function prototypes
void processInput(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height);
void showFPS(GLFWwindow* window);
bool initOpenGL();
void renderScene(Shader & shader);

//-----------------------------------------------------------------------------
// Models
//-----------------------------------------------------------------------------
std::shared_ptr<Model> pObjEarth, pObjMoon;

//-----------------------------------------------------------------------------
// Main Application Entry Point
//-----------------------------------------------------------------------------
int main() {

	if (!initOpenGL()){
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return -1;
	}

	// Shader loader
	Shader objectShader, skyboxShader, shadowShader;
	objectShader.loadShaders("shaders/object.vert",  "shaders/object.frag");
	skyboxShader.loadShaders("shaders/skybox.vert", "shaders/skybox.frag");
	shadowShader.loadShaders("shaders/shadow.vert", "shaders/shadow.frag");



	// Model loader
	pObjEarth = std::make_shared<Model> ("Resources/earth/earth.obj");
	pObjMoon  = std::make_shared<Model> ("Resources/planet/planet.obj");

	// Shadow
	ParallelShadow shadowMap;



	/** Skybox Mapping Order
	      _______
	      | Top |
	      |  3  |
	______|_____|____________
	|Left |Front|Right| Back|
	|  2  |  5  |  1  |  6  |
	|_____|_____|_____|_____|
	      |Botom|
	      |  4  |
	      |_____|
	*/

	Skybox skybox;
	std::vector<std::string> faces = {
		"Resources/skyboxes/universe/image_3.png", // right
		"Resources/skyboxes/universe/image_1.png", // left
		"Resources/skyboxes/universe/image_4.png", // top
		"Resources/skyboxes/universe/image_5.png", // bottom
		"Resources/skyboxes/universe/image_2.png", // front
		"Resources/skyboxes/universe/image_0.png", // back
	};
	skybox.LoadTexture(faces);



	// Light global
	glm::vec3 pointLightPos[] = {
		glm::vec3( 3.0f,  0.0f,  0.0f),
		glm::vec3(-3.0f,  0.0f,  0.0f),
		glm::vec3( 0.0f,  0.0f, -3.0f),
		glm::vec3( 0.0f,  0.0f,  3.0f)
	};
	glm::vec3 directionalLightDirection(-1.0f, 0.0f, 0.0f);

	// Object shader config
	objectShader.use();
	// Light config
	// Directional light
	objectShader.setUniform("uDirectionalLight.direction", directionalLightDirection);
	objectShader.setUniform("uDirectionalLight.ambient", 0.0f, 0.0f, 0.0f);
	objectShader.setUniform("uDirectionalLight.diffuse", 1.0f, 1.0f, 1.0f);
	objectShader.setUniform("uDirectionalLight.specular", 0.0f, 0.0f, 0.0f);
	// Spot light
	objectShader.setUniform("uSpotLight.innerCutOff", glm::cos(glm::radians(12.5f)));
	objectShader.setUniform("uSpotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
	objectShader.setUniform("uSpotLight.ambient", 0.0f, 0.0f, 0.0f);
	objectShader.setUniform("uSpotLight.diffuse", 1.0f, 1.0f, 1.0f);
	objectShader.setUniform("uSpotLight.specular", 1.0f, 1.0f, 1.0f);
	objectShader.setUniform("uSpotLight.constant", 1.0f);
	objectShader.setUniform("uSpotLight.linear", 0.09f);
	objectShader.setUniform("uSpotLight.quadratic", 0.032f);



	// Camera global
	float aspect = (float)gWindowWidth / (float)gWindowHeight;



	// Rendering loop
	while (!glfwWindowShouldClose(gWindow)) {

		// Display FPS on title
		showFPS(gWindow);

		// Key input
		processInput(gWindow);



		// Create transformations
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.fov), aspect, 0.1f, 100.0f);
		//glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 100.0f);



		/** Shadow */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// get light transformation
		glm::mat4 lightProjection, lightView;
		float near_plane = 1.0f, far_plane = 100.0f;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(glm::vec3(50.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		// render scene from light's point of view
		shadowShader.use();
		shadowShader.setUniform("uView", lightView);
		shadowShader.setUniform("uProjection", lightProjection);
		// get shadow map
		glViewport(0, 0, shadowMap.width, shadowMap.height);
		shadowMap.Bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		renderScene(shadowShader);
		glCullFace(GL_BACK);
		shadowMap.Unbind();



		/** General scene */
		#ifdef __APPLE__
		glViewport(0, 0, 2 * gWindowWidth, 2 * gWindowHeight);
		#else
		glViewport(0, 0, gWindowWidth, gWindowHeight);
		#endif
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Skybox
		glm::mat4 staticView = glm::mat4(glm::mat3(view)); // remove translation composition
		skybox.Draw(skyboxShader, staticView, projection);
		// Object shader
		objectShader.use();
		objectShader.setUniform("uView", view);
		objectShader.setUniform("uProjection", projection);
		objectShader.setUniform("uCameraPos", camera.position);
		// Spot light
		objectShader.setUniform("uEnableTorch", enableTorch);
		objectShader.setUniform("uSpotLight.position",  camera.position);
		objectShader.setUniform("uSpotLight.direction", camera.front);
		// Shadow map
		objectShader.setUniform("uLightSpaceMatrix", lightProjection * lightView);
		objectShader.setUniform("uShadowMap", (int) shadowMap.active_texture_unit);
		// Draw scene
		glActiveTexture(GL_TEXTURE0 + shadowMap.active_texture_unit);
		glBindTexture(GL_TEXTURE_2D, shadowMap.TID());
		renderScene(objectShader);



		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwPollEvents();
		glfwSwapBuffers(gWindow);
	}
	
	glfwTerminate();

	return 0;
}

void renderScene(Shader & shader) {

	// Set geological configurations
	float currentTime = (float)glfwGetTime();
	float angularVelocity = glm::radians(10.0f);
	// earth
	glm::vec3 spinAxis(
		glm::sin(glm::radians(23.5f)) * glm::cos(currentTime * angularVelocity / 30.0f),
		glm::cos(glm::radians(23.5f)),
		glm::sin(glm::radians(23.5f)) * glm::sin(currentTime * angularVelocity / 30.0f));
	glm::vec3 deviateAxis(
		glm::sin(currentTime * angularVelocity / 30.0f),
		0.0f,
		-glm::cos(currentTime * angularVelocity / 30.0f));
	// moon
	float moonTrjRadius = 20.0f;
	glm::vec3 moonPos(
		moonTrjRadius * glm::cos(currentTime * angularVelocity / 1.0f),
		0.0f,
		moonTrjRadius * glm::sin(currentTime * angularVelocity / 1.0f));
	
	// Draw scene
	glm::mat4 modelMatrix;

	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, -1.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.02f, 0.02f, 0.02f));
	modelMatrix = glm::rotate(modelMatrix, currentTime * angularVelocity, spinAxis);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(23.5f), deviateAxis);

	shader.use();
	shader.setUniform("uModel", modelMatrix);
	if (enableNormal)
		shader.setUniform("uEnableNormal", true);
	shader.setUniform("uEnableEmission", true);
	pObjEarth.get()->Draw(shader);

	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, moonPos);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
	modelMatrix = glm::rotate(modelMatrix, currentTime * angularVelocity, glm::vec3(0.0f, 1.0f, 0.0f));

	shader.use();
	shader.setUniform("uModel", modelMatrix);
	shader.setUniform("uEnableNormal", false);
	shader.setUniform("uEnableEmission", false);
	pObjMoon.get()->Draw(shader);
}

//-----------------------------------------------------------------------------
// Initialize GLFW and OpenGL
//-----------------------------------------------------------------------------
bool initOpenGL() {

	// Intialize GLFW 
	// GLFW is configured.  Must be called before calling any GLFW functions
	if (!glfwInit()) {
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// forward compatible with newer versions of OpenGL as they become available
	// but not backward compatible (it will not run on devices that do not support OpenGL 3.3
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create an OpenGL 3.3 core, forward compatible context window
	gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);
	if (gWindow == NULL) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	// Make the window's context the current one
	glfwMakeContextCurrent(gWindow);

	// Set the required callback functions
	//glfwSetKeyCallback(gWindow, glfw_onKey);
	glfwSetCursorPosCallback(gWindow, mouseCallback);
	glfwSetScrollCallback(gWindow, scrollCallback);
	glfwSetFramebufferSizeCallback(gWindow, glfw_onFramebufferSize);

	// Initialize GLAD: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	// Define the viewport dimensions
	//glViewport(0, 0, gWindowWidth, gWindowHeight);

	// Depth test
	glEnable(GL_DEPTH_TEST);

	// Face culling
	glEnable(GL_CULL_FACE);

	// Blending
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Hide the cursor and capture it
	glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

//-----------------------------------------------------------------------------
// Is called whenever a key is pressed/released via GLFW
//-----------------------------------------------------------------------------
void processInput(GLFWwindow* window) {

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.processAccerlate(true);
	else
		camera.processAccerlate(false);

	static float deltaTime = 0.0f;
	static float lastFrame = 0.0f;
	// Per-frame time
	float currentFrame = (float) glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.processKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera.processKeyboard(DOWN, deltaTime);
	
	static bool gWireframe = false;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		gWireframe = !gWireframe;
		if (gWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		enableTorch = !enableTorch;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		enableNormal = !enableNormal;
}

//-----------------------------------------------------------------------------
// Is called whenever mouse movement is detected via GLFW
//-----------------------------------------------------------------------------
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {

	static bool  firstMouse = true;
	static float lastX = gWindowWidth / 2;
	static float lastY = gWindowHeight / 2;

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coord range from buttom to top
	lastX = xpos;
	lastY = ypos;

	camera.processMouse(xoffset, yoffset);
}

//-----------------------------------------------------------------------------
// Is called whenever scroller is detected via GLFW
//-----------------------------------------------------------------------------
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.processScroll(yoffset);
}

//-----------------------------------------------------------------------------
// Is called when the window is resized
//-----------------------------------------------------------------------------
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//-----------------------------------------------------------------------------
// Code computes the average frames per second, and also the average time it takes
// to render one frame.  These stats are appended to the window caption bar.
//-----------------------------------------------------------------------------
void showFPS(GLFWwindow* window)
{
	static double previousSeconds = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime(); // returns number of seconds since GLFW started, as double float

	elapsedSeconds = currentSeconds - previousSeconds;

	// Limit text updates to 4 times per second
	if (elapsedSeconds > 0.25)
	{
		previousSeconds = currentSeconds;
		double fps = (double)frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		// The C++ way of setting the window title
		std::ostringstream outs;
		outs.precision(3);	// decimal places
		outs << std::fixed
			<< APP_TITLE << "    "
			<< "FPS: " << fps << "    "
			<< "Frame Time: " << msPerFrame << " (ms)";
		glfwSetWindowTitle(window, outs.str().c_str());

		// Reset for next average.
		frameCount = 0;
	}

	frameCount++;
}
