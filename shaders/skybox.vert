#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 SkyboxCoords;

uniform mat4 uProjection;
uniform mat4 uView;

void main() {

	SkyboxCoords = aPos;
	vec4 pos = uProjection * uView * vec4(aPos, 1.0);

	/** Set z = 1.0 the maximum depth value */
	gl_Position = pos.xyww;
}