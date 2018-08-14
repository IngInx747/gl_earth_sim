#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent; 

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
	vec4 FragPosLightSpace;
	mat3 TBN;
} vs_out;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uLightSpaceMatrix;

void main() {

	gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0f);

	mat3 normalMatrix = transpose(inverse(mat3(uModel)));

	// To transform a vector V's components in tangent space to world space, TBN * V
	vec3 T = normalize(normalMatrix * aTangent);
	vec3 B = normalize(normalMatrix * aBitangent);
	vec3 N = normalize(normalMatrix * aNormal);

	vs_out.FragPos = vec3(uModel * vec4(aPos, 1.0));
	vs_out.Normal = normalMatrix * aNormal;
	vs_out.TexCoords = aTexCoords;
	vs_out.FragPosLightSpace = uLightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
	vs_out.TBN = mat3(T, B, N);
}