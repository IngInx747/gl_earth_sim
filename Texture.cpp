#include <Texture.h>

/** Only include this once */
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

std::unordered_map<TextureType, std::string> TextureTypeName = {
	std::pair<TextureType, std::string> (TEX_UNKNOWN,  "texture_unknown"),
	std::pair<TextureType, std::string> (TEX_DIFFUSE,  "texture_diffuse"),
	std::pair<TextureType, std::string> (TEX_SPECULAR, "texture_specular"),
	std::pair<TextureType, std::string> (TEX_NORMAL,   "texture_normal"),
	std::pair<TextureType, std::string> (TEX_HEIGHT,   "texture_height"),
	std::pair<TextureType, std::string> (TEX_EMISSION, "texture_emission"),
	std::pair<TextureType, std::string> (TEX_AMBIENT,  "texture_ambient")
};

unsigned int LoadTexture(const std::string filename, bool gamma) {

	unsigned int textureID{};
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char * data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

	if (!data)
		std::cerr << "LoadTexture: Texture failed to load at path: " << filename << "\n";

	else {
		GLenum imageFormat;
		if (nrComponents == 1) imageFormat = GL_RED;
		else if (nrComponents == 3) imageFormat = GL_RGB;
		else if (nrComponents == 4) imageFormat = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	stbi_image_free(data);

	return textureID;
}

unsigned int LoadCubemap(const std::vector<std::string> & faces) {

	/**
	* loads a cubemap texture from 6 individual texture faces
	* order:
	* +X (right)
	* -X (left)
	* +Y (top)
	* -Y (bottom)
	* +Z (front) 
	* -Z (back)
	*/

	unsigned int textureID{};
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;

	for (unsigned int i=0; i<faces.size(); i++) {

		unsigned char * data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);

		if (!data)
			std::cerr << "LoadCubemap: Texture failed to load at path: " << faces[i] << "\n";

		else {
			GLenum imageFormat;
			if (nrComponents == 1) imageFormat = GL_RED;
			else if (nrComponents == 3) imageFormat = GL_RGB;
			else if (nrComponents == 4) imageFormat = GL_RGBA;

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, imageFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);
		}

		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

/** Default texture */

std::string defaultTextureFilename("Resources/default/default.png");

Texture defaultDiffuseTexture  {0, TEX_DIFFUSE, defaultTextureFilename};
Texture defaultSpecularTexture {0, TEX_SPECULAR, defaultTextureFilename};
/**
Texture defaultNormalTexture   {0, TEX_NORMAL, defaultTextureFilename};
Texture defaultHeightTexture   {0, TEX_HEIGHT, defaultTextureFilename};
Texture defaultEmissionTexture {0, TEX_EMISSION, defaultTextureFilename};
*/
Texture defaultUnknownTexture  {0, TEX_UNKNOWN, defaultTextureFilename};

Texture DefaultTexture(TextureType type) {
	if (type == defaultDiffuseTexture.type) {
		if (defaultDiffuseTexture.id == 0)
			defaultDiffuseTexture.id = LoadTexture(defaultDiffuseTexture.path);
		return defaultDiffuseTexture;
	} else if (type == defaultSpecularTexture.type) {
		if (defaultSpecularTexture.id == 0)
			defaultSpecularTexture.id = LoadTexture(defaultSpecularTexture.path);
		return defaultSpecularTexture;
	} /**else if (type == defaultNormalTexture.type) {
		if (defaultNormalTexture.id == 0)
			defaultNormalTexture.id = LoadTexture(defaultNormalTexture.path);
		return defaultNormalTexture;
	} else if (type == defaultHeightTexture.type) {
		if (defaultHeightTexture.id == 0)
			defaultHeightTexture.id = LoadTexture(defaultHeightTexture.path);
		return defaultHeightTexture;
	} else if (type == defaultEmissionTexture.type) {
		if (defaultEmissionTexture.id == 0)
			defaultEmissionTexture.id = LoadTexture(defaultEmissionTexture.path);
		return defaultEmissionTexture;
	}*/ else {
		if (defaultUnknownTexture.id == 0)
			defaultUnknownTexture.id = LoadTexture(defaultUnknownTexture.path);
		return defaultUnknownTexture;
	}
}
