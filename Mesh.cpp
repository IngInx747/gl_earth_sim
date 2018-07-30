#include <Mesh.h>
#include <ShaderProgram.h>
#include <Texture.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>

Mesh :: Mesh(
	std::vector<Vertex> vertices,
	std::vector<GLuint> indices,
	std::vector<Texture> textures) :
vertices(vertices), indices(indices), textures(textures) {
	
	setup();
}

void Mesh :: setup() {

	glGenBuffers(1, &vbo); // Generate an empty vertex buffer on the GPU
	glGenBuffers(1, &ebo);
	glGenVertexArrays(1, &vao); // Tell OpenGL to create new Vertex Array Object
	
	glBindVertexArray(vao); // Make the vertices buffer the current one
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // "bind" or set as the current buffer we are working with
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

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

void Mesh :: draw(ShaderProgram & shader) {

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

void Mesh :: deleteBuffers() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}
