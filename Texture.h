#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <unordered_map>

enum TextureType {
	TEX_UNKNOWN,
	TEX_DIFFUSE,
	TEX_SPECULAR,
	TEX_NORMAL,
	TEX_HEIGHT,
	TEX_EMISSION
};

struct Texture {
	unsigned int id;
	TextureType type;
	std::string path;
};

extern std::unordered_map<TextureType, std::string> TextureTypeName;

/** Methods */

unsigned int TextureFromFile(const std::string textureFile, bool gamma = false);
Texture DefaultTexture(TextureType type);

#endif