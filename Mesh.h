#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <ShaderProgram.h>
#include <Texture.h>

struct Pixel {
	glm::vec2 position;
	glm::vec2 texCoords;
};

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	glm::vec3 tangent;
	glm::vec3 bitangent;
};

class Mesh {

public:
	/** Mesh Data */
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	/** Methods */
	Mesh(std::vector<Vertex> vertices,
		std::vector<unsigned int> indices,
		std::vector<Texture> textures);
	//~Mesh();

	void draw(ShaderProgram & shader);
	void deleteBuffers();

private:
	/** Render Data */
	GLuint vbo, ebo, vao;

	/** Methods */
	void setup();
};

#endif