#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <string>
#include <unordered_map>

enum TextureType {
	TEX_UNKNOWN,
	TEX_DIFFUSE,
	TEX_SPECULAR,
	TEX_NORMAL,
	TEX_HEIGHT,
	TEX_EMISSION,
	TEX_AMBIENT
};

struct Texture {
	unsigned int id;
	TextureType type;
	std::string path;
};

extern std::unordered_map<TextureType, std::string> TextureTypeName;

/** Methods */

unsigned int LoadTexture(const std::string textureFile, bool gamma = false);
unsigned int LoadCubemap(const std::vector<std::string> & faces);
Texture DefaultTexture(TextureType type);

#endif