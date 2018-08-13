#include <ShaderProgram.h>
#include <fstream>
#include <iostream>
#include <sstream>

using std::string;

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Shader :: Shader()
	: mHandle(0)
{}

Shader :: Shader(
		const char* vsFilename,
		const char* fsFilename,
		const char* gsFilename)
	: mHandle(0)
{
	loadShaders(vsFilename, fsFilename, gsFilename);
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
Shader :: ~Shader()
{
	// Delete the program
	glDeleteProgram(mHandle);
}

//-----------------------------------------------------------------------------
// Load vertex and fragment shaders, and if geometry shader exists
//-----------------------------------------------------------------------------
bool Shader::loadShaders(
	const char* vsFilename,
	const char* fsFilename,
	const char* gsFilename)
{
	mHandle = glCreateProgram();
	if (mHandle == 0) {
		std::cerr << "Unable to create shader program!" << std::endl;
		return false;
	}

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	std::string vsString = fileToString(vsFilename);
	const GLchar* vsSourcePtr = vsString.c_str();
	glShaderSource(vs, 1, &vsSourcePtr, NULL);
	glCompileShader(vs);
	checkCompileErrors(vs, VERTEX);
	glAttachShader(mHandle, vs);

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fsString = fileToString(fsFilename);
	const GLchar* fsSourcePtr = fsString.c_str();
	glShaderSource(fs, 1, &fsSourcePtr, NULL);
	glCompileShader(fs);
	checkCompileErrors(fs, FRAGMENT);
	glAttachShader(mHandle, fs);

	GLuint gs;
	if (gsFilename) {
		gs = glCreateShader(GL_GEOMETRY_SHADER);
		std::string gsString = fileToString(gsFilename);
		const GLchar* gsSourcePtr = gsString.c_str();
		glShaderSource(gs, 1, &gsSourcePtr, NULL);
		glCompileShader(gs);
		checkCompileErrors(gs, GEOMETRY);
		glAttachShader(mHandle, gs);
	}

	glLinkProgram(mHandle);
	checkCompileErrors(mHandle, PROGRAM);

	glDeleteShader(vs);
	glDeleteShader(fs);
	if (gsFilename)
		glDeleteShader(gs);

	mUniformLocations.clear();

	return true;
}

//-----------------------------------------------------------------------------
// Opens and reads contents of ASCII file to a string.  Returns the string.
// Not good for very large files.
//-----------------------------------------------------------------------------
string Shader :: fileToString(const string& filename)
{
	std::stringstream ss;
	std::ifstream file;

	try
	{
		file.open(filename, std::ios::in);

		if (!file.fail())
		{
			// Using a std::stringstream is easier than looping through each line of the file
			ss << file.rdbuf();
		}

		file.close();
	}
	catch (std::exception ex)
	{
		std::cerr << "Error reading shader filename!" << std::endl;
	}

	return ss.str();
}

//-----------------------------------------------------------------------------
// Activate the shader program
//-----------------------------------------------------------------------------
void Shader :: use()
{
	if (mHandle > 0)
		glUseProgram(mHandle);
}

//-----------------------------------------------------------------------------
// Checks for shader compiler errors
//-----------------------------------------------------------------------------
void  Shader :: checkCompileErrors(GLuint shader, ShaderType type)
{
	int status = 0;

	if (type == PROGRAM)
	{
		glGetProgramiv(mHandle, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
		{
			GLint length = 0;
			glGetProgramiv(mHandle, GL_INFO_LOG_LENGTH, &length);

			// The length includes the NULL character
			string errorLog(length, ' ');	// Resize and fill with space character
			glGetProgramInfoLog(mHandle, length, &length, &errorLog[0]);
			std::cerr << "Error! Shader program failed to link. " << errorLog << std::endl;
		}
	}
	else
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE)
		{
			GLint length = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

			// The length includes the NULL character
			string errorLog(length, ' ');  // Resize and fill with space character
			glGetShaderInfoLog(shader, length, &length, &errorLog[0]);
			std::cerr << "Error! Shader failed to compile. " << errorLog << std::endl;
		}
	}

}

//-----------------------------------------------------------------------------
// Returns the active shader program
//-----------------------------------------------------------------------------
GLuint Shader :: ID() const
{
	return mHandle;
}

//-----------------------------------------------------------------------------
// Sets a boolean shader uniform
//-----------------------------------------------------------------------------
void Shader :: setUniform(const string& name, bool value)
{
	GLint loc = getUniformLocation(name.c_str());
	glUniform1i(loc, (int) value);
}

//-----------------------------------------------------------------------------
// Sets an integer shader uniform
//-----------------------------------------------------------------------------
void Shader :: setUniform(const string& name, int value)
{
	GLint loc = getUniformLocation(name.c_str());
	glUniform1i(loc, value);
}

//-----------------------------------------------------------------------------
// Sets a float shader uniform
//-----------------------------------------------------------------------------
void Shader :: setUniform(const string& name, float value)
{
	GLint loc = getUniformLocation(name.c_str());
	glUniform1f(loc, value);
}

//-----------------------------------------------------------------------------
// Sets a glm::vec2 shader uniform
//-----------------------------------------------------------------------------
void Shader :: setUniform(const string& name, const glm::vec2& v)
{
	GLint loc = getUniformLocation(name.c_str());
	glUniform2fv(loc, 1, &v[0]);
}

void Shader :: setUniform(const string& name, float x, float y)
{
	GLint loc = getUniformLocation(name.c_str());
	glUniform2f(loc, x, y);
}

//-----------------------------------------------------------------------------
// Sets a glm::vec3 shader uniform
//-----------------------------------------------------------------------------
void Shader :: setUniform(const string& name, const glm::vec3& v)
{
	GLint loc = getUniformLocation(name.c_str());
	glUniform3fv(loc, 1, &v[0]);
}

void Shader :: setUniform(const string& name, float x, float y, float z)
{
	GLint loc = getUniformLocation(name.c_str());
	glUniform3f(loc, x, y, z);
}

//-----------------------------------------------------------------------------
// Sets a glm::vec4 shader uniform
//-----------------------------------------------------------------------------
void Shader :: setUniform(const string& name, const glm::vec4& v)
{
	GLint loc = getUniformLocation(name.c_str());
	glUniform4fv(loc, 1, &v[0]);
}

void Shader :: setUniform(const string& name, float x, float y, float z, float w)
{
	GLint loc = getUniformLocation(name.c_str());
	glUniform4f(loc, x, y, z, w);
}

//-----------------------------------------------------------------------------
// Sets a glm::mat2 shader uniform
//-----------------------------------------------------------------------------
void Shader :: setUniform(const string& name, const glm::mat2& m)
{
	GLint loc = getUniformLocation(name.c_str());
	glUniformMatrix2fv(loc, 1, GL_FALSE, &m[0][0]);
}

//-----------------------------------------------------------------------------
// Sets a glm::mat3 shader uniform
//-----------------------------------------------------------------------------
void Shader :: setUniform(const string& name, const glm::mat3& m)
{
	GLint loc = getUniformLocation(name.c_str());
	glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
}

//-----------------------------------------------------------------------------
// Sets a glm::mat4 shader uniform
//-----------------------------------------------------------------------------
void Shader :: setUniform(const string& name, const glm::mat4& m)
{
	GLint loc = getUniformLocation(name.c_str());
	glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
}

//-----------------------------------------------------------------------------
// Returns the uniform identifier given it's string name.
// NOTE: Shader must be currently active first.
//-----------------------------------------------------------------------------
GLint Shader :: getUniformLocation(const GLchar* name)
{
	std::map<string, GLint>::iterator it = mUniformLocations.find(name);

	// Only need to query the shader program IF it doesn't already exist.
	if (it == mUniformLocations.end())
	{
		// Find it and add it to the map
		mUniformLocations[name] = glGetUniformLocation(mHandle, name);
	}

	// Return it
	return mUniformLocations[name];
}
