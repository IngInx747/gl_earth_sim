#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <map>
#include <glad/glad.h>
#include <glm/glm.hpp>

class ShaderProgram {

public:
	 ShaderProgram();
	~ShaderProgram();

	enum ShaderType
	{
		VERTEX,
		FRAGMENT,
		PROGRAM
	};

	// Only supports vertex and fragment (this series will only have those two)
	bool loadShaders(const char* vsFilename, const char* fsFilename);
	void use();

	GLuint getProgram() const;

	void setUniform(const std::string& name, bool value);
	void setUniform(const std::string& name, int value);
	void setUniform(const std::string& name, float value);
	void setUniform(const std::string& name, float x, float y);
	void setUniform(const std::string& name, float x, float y, float z);
	void setUniform(const std::string& name, float x, float y, float z, float w);
	void setUniform(const std::string& name, const glm::vec2& v);
	void setUniform(const std::string& name, const glm::vec3& v);
	void setUniform(const std::string& name, const glm::vec4& v);
	void setUniform(const std::string& name, const glm::mat2& m);
	void setUniform(const std::string& name, const glm::mat3& m);
	void setUniform(const std::string& name, const glm::mat4& m);

private:

	std::string fileToString(const std::string& filename);
	void  checkCompileErrors(GLuint shader, ShaderType type);
	// We are going to speed up looking for uniforms by keeping their locations in a map
	GLint getUniformLocation(const GLchar * name);
	
	GLuint mHandle; // unsigned int
	std::map<std::string, GLint> mUniformLocations;
};

#endif // SHADER_H
