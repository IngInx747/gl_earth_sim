#version 330 core

/** Directional Light */

struct Directional_Light_t {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

vec4 CalcDirectionalLight(Directional_Light_t light, vec3 normal, vec3 viewDir,
	sampler2D diffuse, sampler2D specular);

/** Point Light */

struct  Point_Light_t {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

vec4 CalcPointLight(Point_Light_t light, vec3 normal, vec3 viewDir,
	sampler2D diffuse, sampler2D specular);

/** Spot Light */

struct Spot_Light_t {
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
	float innerCutOff;
	float outerCutOff;
};

vec4 CalcSpotLight(Spot_Light_t light, vec3 normal, vec3 viewDir,
	sampler2D diffuse, sampler2D specular);

vec4 CalcEmission(sampler2D emission, vec4 testLight);

/** Texture mapping */

struct TextureMap_t {
	// diffuse
	sampler2D texture_diffuse1;
	sampler2D texture_diffuse2;
	// specular
	sampler2D texture_specular1;
	sampler2D texture_specular2;
	// normal
	sampler2D texture_normal1;
	sampler2D texture_normal2;
	// height
	sampler2D texture_height1;
	sampler2D texture_height2;
	// emission
	sampler2D texture_emission1;
	sampler2D texture_emission2;
	// To be added ...
};

/** Uniform variables */

// Camera
uniform vec3 uCameraPos;

// Lighting
#define NR_POINT_LIGHTS 4
uniform Directional_Light_t uDirectionalLight;
uniform Spot_Light_t uSpotLight;
uniform Point_Light_t uPointLights[NR_POINT_LIGHTS];

// Texture (Model Importer specified)
uniform TextureMap_t uMaterial;

/** Stream variables */

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main() {

	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(uCameraPos - FragPos);
	vec4 resultColor = vec4(0.0);

	// Directional lighting
	vec4 directionalLightColor = CalcDirectionalLight(uDirectionalLight, normal, viewDir,
		uMaterial.texture_diffuse1, uMaterial.texture_specular1);

	// Spot lighting
	vec4 spotLightColor = CalcSpotLight(uSpotLight, normal, viewDir,
		uMaterial.texture_diffuse1, uMaterial.texture_specular1);

	// Point lighting
	/**
	for (int i=0; i<NR_POINT_LIGHTS; i++) {
		resultColor += CalcPointLight(uPointLights[i], normal, viewDir,
			uMaterial.texture_diffuse1, uMaterial.texture_specular1);
	}*/

	//
	vec4 emissionLight = CalcEmission(uMaterial.texture_emission1, directionalLightColor);

	// Light sum
	resultColor = directionalLightColor + spotLightColor;

	// Greyscale process
	vec4 testColor = texture(uMaterial.texture_diffuse1, TexCoords);
	if (testColor.x > 0.001 && testColor.y < 0.001 && testColor.z < 0.001) {
		// Cloud part
		resultColor = vec4(resultColor.r);
	} else if (testColor.x < 0.001 && testColor.y < 0.001 && testColor.z < 0.001) {
		// Transparent part
		resultColor.a = 0.0;
	}

	// Result
	FragColor = resultColor + emissionLight;
}

vec4 CalcDirectionalLight(Directional_Light_t light, vec3 normal, vec3 viewDir,
	sampler2D diffuse, sampler2D specular) {

	vec4 ambientColor, diffuseColor, specularColor;

	vec3 lightDir = normalize(-light.direction);

	// ambient
	ambientColor = vec4(light.ambient, 1.0) * texture(diffuse, TexCoords);

	// diffuse
	float diffEff = max(dot(normal, lightDir), 0.0);
	diffuseColor = diffEff * vec4(light.diffuse, 1.0) * texture(diffuse, TexCoords);

	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float discardFactor = dot(reflectDir, normal);
	if (discardFactor < 0.0)
		specularColor = vec4(0.0, 0.0, 0.0, 1.0);
	else {
		float specEff = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
		specularColor = specEff * vec4(light.specular, 1.0) * texture(specular, TexCoords);
	}

	// result
	return ambientColor + diffuseColor + specularColor;
}

vec4 CalcPointLight(Point_Light_t light, vec3 normal, vec3 viewDir,
	sampler2D diffuse, sampler2D specular) {

	vec4 ambientColor, diffuseColor, specularColor;

	vec3 lightDir = normalize(light.position - FragPos);

	// Physics
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear*distance + light.quadratic*distance*distance);

	// ambient
	ambientColor = vec4(light.ambient, 1.0) * texture(diffuse, TexCoords);

	// diffuse
	float diffEff = max(dot(normal, lightDir), 0.0);
	diffuseColor = diffEff * vec4(light.diffuse, 1.0) * texture(diffuse, TexCoords);

	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float discardFactor = dot(reflectDir, normal);
	if (discardFactor < 0.0)
		specularColor = vec4(0.0, 0.0, 0.0, 1.0);
	else {
		float specEff = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
		specularColor = specEff * vec4(light.specular, 1.0) * texture(specular, TexCoords);
	}

	// result
	return attenuation * (ambientColor + diffuseColor + specularColor);
}

vec4 CalcSpotLight(Spot_Light_t light, vec3 normal, vec3 viewDir,
	sampler2D diffuse, sampler2D specular) {

	vec4 ambientColor, diffuseColor, specularColor;

	vec3 lightDir = normalize(light.position - FragPos);

	// Physics
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear*distance + light.quadratic*distance*distance);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.innerCutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	// Ambient lighting
	ambientColor = vec4(light.ambient, 1.0) * texture(diffuse, TexCoords);

	// Diffuse lighting
	float diffEff = max(dot(normal, lightDir), 0.0);
	diffuseColor = diffEff * vec4(light.diffuse, 1.0) * texture(diffuse, TexCoords);

	// Specular lighting
	vec3 reflectDir = reflect(-lightDir, normal);
	float discardFactor = dot(reflectDir, normal);
	if (discardFactor < 0.0)
		specularColor = vec4(0.0, 0.0, 0.0, 1.0);
	else {
		float specEff = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
		specularColor = specEff * vec4(light.specular, 1.0) * texture(specular, TexCoords);
	}

	// Result lighting
	return attenuation * (ambientColor + (diffuseColor + specularColor) * intensity);
}

vec4 CalcEmission(sampler2D emission, vec4 testLight) {

	// emission
	vec4 emissionColor = texture(emission, TexCoords);

	// Ignore black parts of emission texture, transparentize them
	if (emissionColor.x < 0.3 || emissionColor.y < 0.3 || emissionColor.z < 0.3)
		emissionColor = vec4(0.0, 0.0, 0.0, 0.0);

	// Turn off light if environment is bright enough (simulate city lighting)
	if (testLight.x > 0.1 || testLight.y > 0.1 || testLight.z > 0.1)
		emissionColor = vec4(0.0, 0.0, 0.0, 0.0);

	return emissionColor;
}
