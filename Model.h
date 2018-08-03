#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <ShaderProgram.h>
#include <Texture.h>
#include <Mesh.h>

class Model {
public:
	/** Methods */
	Model(std::string path, bool gamma = false);
	~Model();
	void draw(ShaderProgram & shader);

	void Translate(glm::vec3 position);
	void Translate(float x, float y, float z);
	void Scale(glm::vec3 scale);
	void Scale(float x, float y, float z);
	void Rotate(float rotation, glm::vec3 axis);

private:
	/** Model Data */
	std::vector<Mesh> meshes;
	std::string directory;
	bool gammaCorrection;
	std::vector<Texture> textures_loaded; // store loaded textures to avoid loading twice

	/** Geometry params */
	glm::vec3 position;
	glm::vec3 scale;
	glm::mat4 rotation;
	unsigned int cnt_translate;
	unsigned int cnt_scale;
	unsigned int cnt_rotate;

	/** Methods */
	void loadModel(std::string & path);
	void processNode(aiNode * node, const aiScene * scene);
	Mesh processMesh(aiMesh * mesh, const aiScene * scene);
	std::vector<Texture> loadTextures(
		aiMaterial * material,
		aiTextureType aiTexType, 
		TextureType type);
};

#endif