#include <Model.h>
#include <Mesh.h>
#include <ShaderProgram.h>
#include <Texture.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <vector>
#include <string>

Model :: Model(std::string path, bool gamma) : gammaCorrection(gamma) {
	loadModel(path);
}

void Model :: Draw(ShaderProgram & shader) {
	for (unsigned int i=0; i<meshes.size(); i++)
		meshes[i].Draw(shader);
}

void Model :: loadModel(std::string & path) {

	/**
	* Loads a model with supported ASSIMP extensions from file
	* and stores resulting meshes in meshes vector
	*/

	// Read file via ASSIMP
	Assimp::Importer importer;
	const aiScene * scene = importer.ReadFile(path,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	/**
	* iProcess_Triangulate : if the model does not (entirely) consist of triangles,
		it should transform all the model's primitive shapes to triangles.

	* aiProcess_FlipUVs : flip texture coordinates on the y-axis where necessary during processing.

	* aiProcess_GenNormals : creates normals for each vertex if the model didn't contain normal vectors.

	* aiProcess_SplitLargeMeshes : splits large meshes into smaller sub-meshes which is useful,
		if your rendering has a maximum number of vertices allowed and can only process smaller meshes.

	* aiProcess_OptimizeMeshes :  does reverse by trying to join several meshes into one larger mesh,
		reducing drawing calls for optimization.
	*/

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << "\n";
		return;
	}

	// Retrieve directory path of filepath
	directory = path.substr(0, path.find_last_of('/')) + "/";

	std::cout << "Model::loadModel: " << directory << "\n";

	// Process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);
}

void Model :: processNode(aiNode * node, const aiScene * scene) {

	/**
	* Process a node in recursive fashion. Process each individual mesh located at node
	* and repeats this process on its children nodes (if any).
	*/
	
	// Process all the nodes' meshes
	for (unsigned int i=0; i<node->mNumMeshes; i++) {
		// Node object only contains indices to index the actual objects in the scene
		// Scene contains all data, node is just to keep stuff organized (like relations between nodes).
		aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	// then do the same for each of its children
	for (unsigned int i=0; i<node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model :: processMesh(aiMesh * mesh, const aiScene * scene) {

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// process vertex positions, normals and texture coords
	for (unsigned int i=0; i<mesh->mNumVertices; i++) {
		
		Vertex vertex;
		glm::vec3 v;

		// Position
		if (mesh->mVertices) {
			v.x = mesh->mVertices[i].x;
			v.y = mesh->mVertices[i].y;
			v.z = mesh->mVertices[i].z;
			vertex.position = v;
		}

		// Normals
		if (mesh->mNormals) {
			v.x = mesh->mNormals[i].x;
			v.y = mesh->mNormals[i].y;
			v.z = mesh->mNormals[i].z;
			vertex.normal = v;
		} else {
			//std::cerr << "Mesh::processMesh: Unable to load Normal: " << i << "\n";
			vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
		}

		// Texture coordinates
		if (mesh->mTextureCoords && mesh->mTextureCoords[0]) { // does mesh contain texture coords?
			glm::vec2 u;
			// A vertex can contain up to 8 texture coords. Assume that one won't use models
			// where a vertex has multiple texture coords. So take first set (0).
			u.x = mesh->mTextureCoords[0][i].x;
			u.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = u;
		} else {
			//std::cerr << "Mesh::processMesh: Unable to load Texture: " << i << "\n";
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
		}

		// Normals
		if (mesh->mTangents) {
			v.x = mesh->mTangents[i].x;
			v.y = mesh->mTangents[i].y;
			v.z = mesh->mTangents[i].z;
			vertex.tangent = v;
		} else {
			//std::cerr << "Mesh::processMesh: Unable to load Tangent: " << i << "\n";
			vertex.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
		}

		// Binormals
		if (mesh->mBitangents) {
			v.x = mesh->mBitangents[i].x;
			v.y = mesh->mBitangents[i].y;
			v.z = mesh->mBitangents[i].z;
			vertex.bitangent = v;
		} else {
			//std::cerr << "Mesh::processMesh: Unable to load Bitangent: " << i << "\n";
			vertex.bitangent = glm::vec3(0.0f, 0.0f, 1.0f);
		}

		vertices.push_back(vertex);
	}

	// process indices
	for (unsigned int i=0; i<mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		// Retrieve all indices of the face and store them in indices vector
		for (unsigned int j=0; j<face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// process material
	if (mesh->mMaterialIndex >= 0) {

		aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];
		// Sampler names in shaders convention: "texture_typeNameN"

		// diffuse maps
		std::vector<Texture> diffuseMaps = loadTextures(material,
			aiTextureType_DIFFUSE, TEX_DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// specular maps
		std::vector<Texture> specularMaps = loadTextures(material,
			aiTextureType_SPECULAR, TEX_SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// normal maps
		std::vector<Texture> normalMaps = loadTextures(material,
			aiTextureType_HEIGHT, TEX_NORMAL);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// height maps
		std::vector<Texture> heightMaps = loadTextures(material,
			aiTextureType_AMBIENT, TEX_HEIGHT);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
		// emission maps
		std::vector<Texture> emissionMaps = loadTextures(material,
			aiTextureType_EMISSIVE, TEX_EMISSION);
		textures.insert(textures.end(), emissionMaps.begin(), emissionMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model :: loadTextures(
	aiMaterial * material,
	aiTextureType aiTexType,
	TextureType type) {

	/**
	* Check all material textures of a given type and loads 
	*/

	std::vector<Texture> textures;
	unsigned int typeCount = material->GetTextureCount(aiTexType);

	for (unsigned int i=0; i<typeCount; i++) {

		aiString str;
		material->GetTexture(aiTexType, i, &str);

		// Check if texture was loaded before. Yes: continue.
		bool skip = false;
		for (unsigned int j=0; j<textures_loaded.size(); j++) {
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip == true; break;
			}
		}
		if (skip) continue;

		Texture texture;
		texture.id = TextureFromFile(directory + std::string(str.C_Str()));
		texture.type = type;
		texture.path = str.C_Str();
		textures.push_back(texture);
		textures_loaded.push_back(texture); // record to avoid repeated loading

		std::cout << "Model::loadTextures: " << texture.id << "\t"
			<< TextureTypeName[texture.type] << "\tfrom: " << texture.path << "\n";
	}

	if (typeCount == 0 && (type == TEX_DIFFUSE)) {
		Texture texture = DefaultTexture(type);
		textures.push_back(texture);
		std::cout << "Model::loadTextures: " << texture.id << "\t"
			<< TextureTypeName[texture.type] << "\tfrom: " << texture.path << "\n";
	}

	return textures;
}
