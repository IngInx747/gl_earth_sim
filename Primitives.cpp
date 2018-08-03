#include <Primitives.h>
#include <Texture.h>
#include <ShaderProgram.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <map>





/*************************************************
*
* 2D Objects
*
*************************************************/

Base2D :: Base2D() {
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	scale    = glm::vec3(1.0f, 1.0f, 1.0f);
	rotation = glm::mat4(1.0f);
}

Base2D :: ~Base2D() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

void Base2D :: setup() {

	glGenBuffers(1, &vbo); // Generate an empty vertex buffer on the GPU
	glGenBuffers(1, &ebo);
	glGenVertexArrays(1, &vao); // Tell OpenGL to create new Pixel Array Object
	
	glBindVertexArray(vao); // Make the vertices buffer the current one
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // "bind" or set as the current buffer we are working with
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Pixel), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // vertex positions
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Pixel), NULL);
	glEnableVertexAttribArray(1); // vertex texture coords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Pixel), (void*)offsetof(Pixel, texCoords));

	glBindVertexArray(0); // Release control of vao
}

void Base2D :: draw(ShaderProgram & shader) {

	// Manipulate transformation operations
	glm::mat4 modelMatrix(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::scale(modelMatrix, scale);
	modelMatrix = rotation * modelMatrix;
	// Clear transformation operations
	cnt_translate = 0;
	cnt_scale = 0;
	cnt_rotate = 0;
	
	// Bind shader with transformation
	shader.use();
	shader.setUniform("uModel", modelMatrix);

	// Bind textures
	for (unsigned int i=0; i<textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		shader.setUniform("uMaterial.texture" + std::to_string(i + 1), (int)i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// Draw mesh
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void Base2D :: addTexture(unsigned int tid) {
	Texture texture;
	texture.id = tid;
	if (texture.id != 0) textures.push_back(texture);
}

void Base2D :: addTexture(const std::string path) {
	
	Texture texture;
	texture.id   = LoadTexture(path);
	texture.path = path;
	if (texture.id != 0) {
		textures.push_back(texture);
		std::cout << "Base2D::loadTextures: " << texture.id << "\t"
			<< TextureTypeName[texture.type] << "\tfrom: " << texture.path << "\n";
	}
}

void Base2D :: Translate(glm::vec3 position) {
	if (cnt_translate == 0)
		this->position = glm::vec3(0.0f, 0.0f, 0.0f);
	this->position += position;
	cnt_translate++;
}

void Base2D :: Translate(float x, float y, float z) {
	if (cnt_translate == 0)
		this->position = glm::vec3(0.0f, 0.0f, 0.0f);
	this->position += glm::vec3(x, y, z);
	cnt_translate++;
}

void Base2D :: Scale(glm::vec2 scale) {
	if (cnt_scale == 0)
		this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
	this->scale *= glm::vec3(scale.x, scale.y, 1.0f);
	cnt_scale++;
}

void Base2D :: Scale(float x, float y) {
	if (cnt_scale == 0)
		this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
	this->scale *= glm::vec3(x, y, 1.0f);
	cnt_scale++;
}

void Base2D :: Rotate(float radian, glm::vec3 axis) {
	if (cnt_rotate == 0)
		this->rotation = glm::mat4(1.0f);
	this->rotation = glm::rotate(this->rotation, radian, axis);
	cnt_rotate++;
}

/*************************************************
* Quad
*************************************************/

std::vector<std::vector<float> > quad_vertices = {
	// positions    // texCoords
	{-1.0f, -1.0f,  0.0f, 0.0f},
	{ 1.0f, -1.0f,  1.0f, 0.0f},
	{ 1.0f,  1.0f,  1.0f, 1.0f},
	{-1.0f,  1.0f,  0.0f, 1.0f}
};

std::vector<unsigned int> quad_elements = {
	0,   1,   2,
	2,   3,   0
};

Quad :: Quad() {

	for (auto item : quad_vertices) {
		Pixel pixel;
		glm::vec2 v;

		v.x = item[0]; v.y = item[1];
		pixel.position = v;

		v.x = item[2]; v.y = item[3];
		pixel.texCoords = v;

		vertices.push_back(pixel);
	}
	
	indices = quad_elements;
	
	setup();
}





/*************************************************
*
* 3D Objects
*
*************************************************/

Base3D :: Base3D() {
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	scale    = glm::vec3(1.0f, 1.0f, 1.0f);
	rotation = glm::mat4(1.0f);
}

Base3D :: ~Base3D() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

void Base3D :: setup() {

	glGenBuffers(1, &vbo); // Generate an empty vertex buffer on the GPU
	glGenBuffers(1, &ebo);
	glGenVertexArrays(1, &vao); // Tell OpenGL to create new Vertex Array Object
	
	glBindVertexArray(vao); // Make the vertices buffer the current one
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // "bind" or set as the current buffer we are working with
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // vertex positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
	glEnableVertexAttribArray(1); // vertex normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2); // vertex texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	glEnableVertexAttribArray(3); // vertex tangent coords
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	glEnableVertexAttribArray(4); // vertex bitangent coords
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

	glBindVertexArray(0); // Release control of vao
}

void Base3D :: draw(ShaderProgram & shader) {

	// Manipulate transformation operations
	glm::mat4 modelMatrix(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::scale(modelMatrix, scale);
	modelMatrix = rotation * modelMatrix;
	// Clear transformation operations
	cnt_translate = 0;
	cnt_scale = 0;
	cnt_rotate = 0;
	
	// Bind shader with transformation
	shader.use();
	shader.setUniform("uModel", modelMatrix);

	// Bind textures
	unsigned int diffuseNr  = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr   = 1;
	unsigned int heightNr   = 1;
	unsigned int emissionNr = 1;
	unsigned int ambientNr  = 1;

	for (unsigned int i=0; i<textures.size(); i++) {

		glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding

		std::string number;
		TextureType type = textures[i].type;

		if (type == TEX_DIFFUSE)
			number = std::to_string(diffuseNr++);
		else if (type == TEX_SPECULAR)
			number = std::to_string(specularNr++);
		else if (type == TEX_NORMAL)
			number = std::to_string(normalNr++);
		else if (type == TEX_HEIGHT)
			number = std::to_string(heightNr++);
		else if (type == TEX_EMISSION)
			number = std::to_string(emissionNr++);
		else if (type == TEX_AMBIENT)
			number = std::to_string(ambientNr++);

		shader.setUniform("uMaterial." + TextureTypeName[type] + number, (int)i);
		// Bind the texture
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// Draw mesh
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void Base3D :: addTexture(unsigned int tid) {
	Texture texture;
	texture.id = tid;
	if (texture.id != 0) textures.push_back(texture);
}

void Base3D :: addTexture(const std::string path, TextureType type) {
	
	Texture texture;
	texture.id   = LoadTexture(path);
	texture.type = type;
	texture.path = path;
	if (texture.id != 0) {
		textures.push_back(texture);
		std::cout << "Base3D::loadTextures: " << texture.id << "\t"
			<< TextureTypeName[texture.type] << "\tfrom: " << texture.path << "\n";
	}
}

void Base3D :: Translate(glm::vec3 position) {
	if (cnt_translate == 0)
		this->position = glm::vec3(0.0f, 0.0f, 0.0f);
	this->position += position;
	cnt_translate++;
}

void Base3D :: Translate(float x, float y, float z) {
	if (cnt_translate == 0)
		this->position = glm::vec3(0.0f, 0.0f, 0.0f);
	this->position += glm::vec3(x, y, z);
	cnt_translate++;
}

void Base3D :: Scale(glm::vec3 scale) {
	if (cnt_scale == 0)
		this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
	this->scale *= scale;
	cnt_scale++;
}

void Base3D :: Scale(float x, float y, float z) {
	if (cnt_scale == 0)
		this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
	this->scale *= glm::vec3(x, y, z);
	cnt_scale++;
}

void Base3D :: Rotate(float radian, glm::vec3 axis) {
	if (cnt_rotate == 0)
		this->rotation = glm::mat4(1.0f);
	this->rotation = glm::rotate(this->rotation, radian, axis);
	cnt_rotate++;
}

/*************************************************
* Plane
*************************************************/

std::vector<std::vector<float> > plane_vertices = {
	// positions        // normals         // texCoords
	{-0.5,  0.0, -0.5,   0.0,  1.0,  0.0,   0.0f, 0.0f},
	{ 0.5,  0.0, -0.5,   0.0,  1.0,  0.0,   1.0f, 0.0f},
	{ 0.5,  0.0,  0.5,   0.0,  1.0,  0.0,   1.0f, 1.0f},
	{-0.5,  0.0,  0.5,   0.0,  1.0,  0.0,   0.0f, 1.0f}
};

std::vector<unsigned int> plane_elements = {
	0,   1,   2,
	2,   3,   0
};

Plane :: Plane() {

	for (auto item : plane_vertices) {
		Vertex vertex;
		glm::vec3 v;
		glm::vec2 u;

		v.x = item[0]; v.y = item[1]; v.z = item[2];
		vertex.position = v;

		v.x = item[3]; v.y = item[4]; v.z = item[5];
		vertex.normal = v;

		u.x = item[6]; u.y = item[7];
		vertex.texCoords = u;

		vertex.tangent = vertex.bitangent = glm::vec3(0.0f);

		vertices.push_back(vertex);
	}
	
	indices = plane_elements;
	
	setup();
}

/*************************************************
* Cube
*************************************************/

std::vector<std::vector<float> > cube_vertices = {
	// positions        // normals         // texCoords
	// front
	{-0.5, -0.5,  0.5,   0.0,  0.0,  1.0,   0.0f, 0.0f},
	{ 0.5, -0.5,  0.5,   0.0,  0.0,  1.0,   1.0f, 0.0f},
	{ 0.5,  0.5,  0.5,   0.0,  0.0,  1.0,   1.0f, 1.0f},
	{-0.5,  0.5,  0.5,   0.0,  0.0,  1.0,   0.0f, 1.0f},
	// back
	{-0.5, -0.5, -0.5,   0.0,  0.0, -1.0,   0.0f, 0.0f},
	{ 0.5, -0.5, -0.5,   0.0,  0.0, -1.0,   1.0f, 0.0f},
	{ 0.5,  0.5, -0.5,   0.0,  0.0, -1.0,   1.0f, 1.0f},
	{-0.5,  0.5, -0.5,   0.0,  0.0, -1.0,   0.0f, 1.0f},
	// left
	{-0.5, -0.5, -0.5,  -1.0,  0.0,  0.0,   0.0f, 0.0f},
	{-0.5, -0.5,  0.5,  -1.0,  0.0,  0.0,   1.0f, 0.0f},
	{-0.5,  0.5,  0.5,  -1.0,  0.0,  0.0,   1.0f, 1.0f},
	{-0.5,  0.5, -0.5,  -1.0,  0.0,  0.0,   0.0f, 1.0f},
	// right
	{ 0.5, -0.5,  0.5,   1.0,  0.0,  0.0,   0.0f, 0.0f},
	{ 0.5, -0.5, -0.5,   1.0,  0.0,  0.0,   1.0f, 0.0f},
	{ 0.5,  0.5, -0.5,   1.0,  0.0,  0.0,   1.0f, 1.0f},
	{ 0.5,  0.5,  0.5,   1.0,  0.0,  0.0,   0.0f, 1.0f},
	// top
	{-0.5,  0.5,  0.5,   0.0,  1.0,  0.0,   0.0f, 0.0f},
	{ 0.5,  0.5,  0.5,   0.0,  1.0,  0.0,   1.0f, 0.0f},
	{ 0.5,  0.5, -0.5,   0.0,  1.0,  0.0,   1.0f, 1.0f},
	{-0.5,  0.5, -0.5,   0.0,  1.0,  0.0,   0.0f, 1.0f},
	// buttom
	{ 0.5, -0.5, -0.5,   0.0, -1.0,  0.0,   0.0f, 0.0f},
	{ 0.5, -0.5,  0.5,   0.0, -1.0,  0.0,   1.0f, 0.0f},
	{-0.5, -0.5,  0.5,   0.0, -1.0,  0.0,   1.0f, 1.0f},
	{-0.5, -0.5, -0.5,   0.0, -1.0,  0.0,   0.0f, 1.0f}
};

std::vector<unsigned int> cube_elements = {
	// front
	0,   1,   2,
	2,   3,   0,
	// back
	4,   5,   6,
	6,   7,   4,
	// left
	8,   9,   10,
	10,  11,  8,
	// right
	12,  13,  14,
	14,  15,  12,
	// top
	16,  17,  18,
	18,  19,  16,
	// buttom
	20,  21,  22,
	22,  23,  20,
};

Cube :: Cube() {

	for (auto item : cube_vertices) {
		Vertex vertex;
		glm::vec3 v;
		glm::vec2 u;

		v.x = item[0]; v.y = item[1]; v.z = item[2];
		vertex.position = v;

		v.x = item[3]; v.y = item[4]; v.z = item[5];
		vertex.normal = v;

		u.x = item[6]; u.y = item[7];
		vertex.texCoords = u;

		vertex.tangent = vertex.bitangent = glm::vec3(0.0f);

		vertices.push_back(vertex);
	}
	
	indices = cube_elements;

	FaceCenters = {
		{ 0.0,  0.0,  0.5}, // front
		{ 0.0,  0.0, -0.5}, // back
		{-0.5,  0.0,  0.0}, // left
		{ 0.5,  0.0,  0.0}, // right
		{ 0.0,  0.5,  0.0}, // top
		{ 0.0, -0.5,  0.0}, // buttom
	};
	
	setup();
}

void Cube :: update(glm::vec3 camPos) {

	std::map<float, int> distdict;

	std::vector<glm::vec3> FaceCentersLocal(FaceCenters);

	for (glm::vec3 & fc : FaceCentersLocal) {
		glm::vec4 fc4(fc.x, fc.y, fc.z, 1.0);
		fc4 = rotation * fc4;
		fc = glm::vec3(fc4.x, fc4.y, fc4.z);
	}

	for (int i=0; i<FaceCentersLocal.size(); i++) {
		glm::vec3 fc = position + FaceCentersLocal[i];
		float distance = glm::length2(camPos - fc);
		if (distdict.find(distance) == distdict.end())
			distdict[distance] = i;
		else
			distdict[distance + 0.01f * i] = i;
	}

	indices.clear();
	std::vector<unsigned int> face_elements = {
		0,   1,   2,
		2,   3,   0
	};

	for (auto it=distdict.rbegin(); it!=distdict.rend(); it++) {
		int face_id = it->second;
		for (int elem : face_elements) 
			indices.push_back(elem + face_id * 4);
	}

	//glBindBuffer(GL_ARRAY_BUFFER, vbo); // "bind" or set as the current buffer we are working with
	//glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}
