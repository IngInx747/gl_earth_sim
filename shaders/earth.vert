#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {

	gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0f);

	// Get one fragment's position in World Space
	FragPos = vec3(uModel * vec4(aPos, 1.0));

	// Also don't forget to transform normal vector
	//Normal = mat3(transpose(inverse(uModel))) * aNormal;
	Normal = mat3(uModel) * aNormal;

	TexCoords = aTexCoords;
}