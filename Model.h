#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <ShaderProgram.h>
#include <Texture.h>
#include <Mesh.h>

class Model
{
public:
	/** Methods */
	Model(std::string path, bool gamma = false);
	~Model();
	void Draw(Shader & shader);

	//void Translate(glm::vec3 trans);
	//void Translate(float x, float y, float z);
	//void Scale(glm::vec3 scale);
	//void Scale(float x, float y, float z);
	//void Rotate(float radian, glm::vec3 axis);

	/** Model Data */
	std::vector<Mesh> meshes;
	std::vector<Texture> textures_loaded; // store loaded textures to avoid loading twice

private:
	/** Model Data */
	std::string directory;
	bool gammaCorrection;

	/** Geometry params */
	//glm::vec3 position;
	//glm::vec3 scale;
	//glm::mat4 rotation;
	//unsigned int cnt_translate;
	//unsigned int cnt_scale;
	//unsigned int cnt_rotate;

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