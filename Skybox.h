#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>

#include <ShaderProgram.h>
#include <glm/glm.hpp>


class Skybox {
public:
	Skybox();

	void Draw(ShaderProgram & shader, glm::mat4 & view, glm::mat4 & projection);
	void LoadTexture(std::vector<std::string> & faces);

private:
	unsigned int vbo, vao, ebo, tid;

	void setup();
};

#endif