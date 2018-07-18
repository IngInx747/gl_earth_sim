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
	void Draw(ShaderProgram & shader);

private:
	/** Model Data */
	std::vector<Mesh> meshes;
	std::string directory;
	bool gammaCorrection;
	std::vector<Texture> textures_loaded; // store loaded textures to avoid loading twice

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