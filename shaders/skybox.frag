#version 330 core

out vec4 FragColor;

in vec3 SkyboxCoords;

uniform samplerCube uSkybox;

void main() {

	FragColor = texture(uSkybox, SkyboxCoords);
}
