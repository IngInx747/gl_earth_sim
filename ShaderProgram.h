#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <map>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader {

public:
	Shader();

	Shader(
		const char* vsFilename,
		const char* fsFilename,
		const char* gsFilename = NULL);

	~Shader();

	enum ShaderType
	{
		VERTEX,
		FRAGMENT,
		GEOMETRY,
		PROGRAM
	};

	void use();

	GLuint ID() const;

	bool loadShaders(
		const char* vsFilename,
		const char* fsFilename,
		const char* gsFilename = NULL);

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

	GLint getUniformLocation(const GLchar * name);
	
	GLuint mHandle;
	std::map<std::string, GLint> mUniformLocations;
};

#endif // SHADER_H
