#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>

#include <ShaderProgram.h>
#include <glm/glm.hpp>


class Skybox {

public:

	const unsigned int active_texture_unit;

	Skybox();

	void Draw(Shader & shader, glm::mat4 & view, glm::mat4 & projection);
	void LoadTexture(std::vector<std::string> & faces);

	unsigned int VBO() { return vbo; }
	unsigned int VAO() { return vao; }
	unsigned int EBO() { return ebo; }
	unsigned int TID() { return tid; }

private:
	unsigned int vbo, vao, ebo, tid;

	void setup();
};

#endif