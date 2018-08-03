#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <ShaderProgram.h>
#include <Texture.h>
#include <Mesh.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

class Base2D {
public:
	/** Render Data */
	std::vector<Pixel>         vertices;
	std::vector<unsigned int>  indices;
	std::vector<Texture>       textures;

	/** Methods */
	Base2D();
	~Base2D();

	void draw(ShaderProgram & shader);
	void addTexture(unsigned int tid);
	void addTexture(const std::string path);
	void deleteBuffers();

	void Translate(glm::vec3 trans);
	void Translate(float x, float y, float z);
	void Scale(glm::vec2 scale);
	void Scale(float x, float y);
	void Rotate(float radian, glm::vec3 axis);

protected:
	/** Render Data */
	unsigned int vbo, ebo, vao;

	/** Geometry params */
	glm::vec3 position;
	glm::vec3 scale;
	glm::mat4 rotation;
	unsigned int cnt_translate;
	unsigned int cnt_scale;
	unsigned int cnt_rotate;

	/** Methods */
	void setup();
};

class Quad : public Base2D {
public:
	/** Methods */
	Quad();
};

class Base3D {
public:
	/** Render Data */
	std::vector<Vertex>        vertices;
	std::vector<unsigned int>  indices;
	std::vector<Texture>       textures;

	/** Methods */
	Base3D();
	~Base3D();

	void draw(ShaderProgram & shader);
	void addTexture(unsigned int tid);
	void addTexture(const std::string path, TextureType type);
	void deleteBuffers();

	void Translate(glm::vec3 trans);
	void Translate(float x, float y, float z);
	void Scale(glm::vec3 scale);
	void Scale(float x, float y, float z);
	void Rotate(float radian, glm::vec3 axis);

protected:
	/** Render Data */
	unsigned int vbo, ebo, vao;

	/** Geometry params */
	glm::vec3 position;
	glm::vec3 scale;
	glm::mat4 rotation;
	unsigned int cnt_translate;
	unsigned int cnt_scale;
	unsigned int cnt_rotate;

	/** Methods */
	void setup();
};

class Plane : public Base3D {
public:
	/** Methods */
	Plane();
};

class Cube : public Base3D {
public:
	Cube();
	void update(glm::vec3 camPos);

protected:
	//enum FaceDir { FRONT, BACK, LEFT, RIGHT, TOP, BUTTOM };
	std::vector<glm::vec3> FaceCenters;
};

#endif