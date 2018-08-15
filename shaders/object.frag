#version 330 core

/** Directional Light */

struct Directional_Light_t {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

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

/** Function definition */

vec4 CalcDirectionalLight(
	Directional_Light_t light,
	vec3 normal, vec3 viewDir,
	vec2 texCoords, sampler2D diffuse, sampler2D specular);

vec4 CalcPointLight(
	Point_Light_t light,
	vec3 normal, vec3 viewDir, vec3 fragPos,
	vec2 texCoords, sampler2D diffuse, sampler2D specular);

vec4 CalcSpotLight(
	Spot_Light_t light,
	vec3 normal, vec3 viewDir, vec3 fragPos,
	vec2 texCoords, sampler2D diffuse, sampler2D specular);

vec4 CalcEmission(
	vec2 texCoords, sampler2D emission,
	vec4 testLight);

float CalcParallelShadow(vec3 lightDir, vec3 normal);

vec2 ParallaxMapping(
	vec2 texCoords, sampler2D height, float scale,
	vec3 viewDir, vec3 normal);

/** Uniform variables */

// Camera
uniform vec3 uCameraPos;

// Lighting
#define NR_POINT_LIGHTS 4
uniform Directional_Light_t uDirectionalLight;
uniform Spot_Light_t uSpotLight;
uniform Point_Light_t uPointLight;
uniform Point_Light_t uPointLights[NR_POINT_LIGHTS];

// Texture (Model Importer specified)
uniform TextureMap_t uMaterial;

// Shadow
uniform sampler2D uShadowMap;

// Control
uniform bool uEnableTorch;
uniform bool uEnableEmission;
uniform bool uEnableNormal;
uniform float uGamma;
uniform float uHeightScale;

/** Stream variables */

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
	vec4 FragPosLightSpace;
	mat3 TBN;
} fs_in;

void main() {

	vec3 normal = normalize(fs_in.Normal);
	vec3 viewDir = normalize(uCameraPos - fs_in.FragPos);

	// Parallax mapping
	vec2 texCoords = fs_in.TexCoords;

	if (uEnableNormal) {
		// get normal map
		normal = texture(uMaterial.texture_normal1, fs_in.TexCoords).rgb;
		normal = normalize(normal * 2.0 - 1.0); // [0,1] -> [-1,1]
		// get parallax map
		texCoords = ParallaxMapping(
			fs_in.TexCoords, uMaterial.texture_height1, uHeightScale,
			transpose(fs_in.TBN) * viewDir, normal);
		// transform normal vector to world space coordinates
		normal = normalize(fs_in.TBN * normal);
	}

	vec4 resultColor = vec4(0.0);

	// Directional lighting
	vec4 directionalLightColor = CalcDirectionalLight(
		uDirectionalLight, normal, viewDir,
		texCoords, uMaterial.texture_diffuse1, uMaterial.texture_specular1);

	// Spot lighting
	vec4 spotLightColor = vec4(0.0);
	if (uEnableTorch)
		spotLightColor = CalcSpotLight(
			uSpotLight, normal, viewDir, fs_in.FragPos,
			texCoords, uMaterial.texture_diffuse1, uMaterial.texture_specular1);

	// Point lighting
	/**
	for (int i=0; i<NR_POINT_LIGHTS; i++) {
		resultColor += CalcPointLight(
			uPointLights[i], normal, viewDir, fs_in.FragPos,
			texCoords, uMaterial.texture_diffuse1, uMaterial.texture_specular1);
	}*/

	//
	vec4 emissionLight = vec4(0.0);
	if (uEnableEmission)
		emissionLight = CalcEmission(
			texCoords, uMaterial.texture_emission1, directionalLightColor);

	// Light sum
	resultColor = directionalLightColor + spotLightColor;

	// Greyscale process
	vec4 testColor = texture(uMaterial.texture_diffuse1, fs_in.TexCoords);
	if (testColor.x > 0.001 && testColor.y < 0.001 && testColor.z < 0.001) {
		// Cloud part
		resultColor = vec4(resultColor.r);
	} else if (testColor.x < 0.001 && testColor.y < 0.001 && testColor.z < 0.001) {
		// Transparent part
		resultColor.a = 0.0;
	}

	// Result
	FragColor = resultColor + emissionLight;

	// Gamma correction
	FragColor.xyz = pow(FragColor.xyz, vec3(1.0 / uGamma));
}

vec2 ParallaxMapping(vec2 texCoords, sampler2D height, float scale, vec3 viewDir, vec3 normal) {
	
	//float disp = texture(height, texCoords).r;     
	//return texCoords - viewDir.xy * (disp * scale);

	float minLayers = 8;
	float maxLayers = 32;
	float numLayers = mix(maxLayers, minLayers, abs(dot(normal, viewDir)));
	// calculate size of each layer
	float layerDepth = 1.0 / numLayers;
	// depth of current layer
	float currentLayerDepth = 0.0;
	// amount to shift texture coordinates per layer
	vec2 P = viewDir.xy * scale;
	vec2 deltaTexCoords = P / numLayers;
	
	vec2 currentTexCoords = texCoords;
	float currentDepthValue = texture(height, currentTexCoords).r;

	for (int i = 0; i < numLayers; i++) {
		if (currentLayerDepth >= currentDepthValue)
			break;
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depth map value at current texture coordinates
		currentDepthValue = texture(height, currentTexCoords).r;
		// get depth of next layer
		currentLayerDepth += layerDepth;
	}

	return currentTexCoords;
}

float CalcParallelShadow(vec3 lightDir, vec3 normal) {
	// perform perspective divide
	vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
	//
	if (projCoords.z > 1.0) return 0.0;
	// transform to [0, 1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light's perspective
	//float closestDepth = texture(shadowMap, projCoords.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// check whether current frag pos is in shadow
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	//float shadow = shadow += (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			float pcfDepth = texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += (currentDepth - bias > pcfDepth) ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	return shadow;
}

vec4 CalcDirectionalLight(Directional_Light_t light, vec3 normal, vec3 viewDir,
	vec2 texCoords, sampler2D diffuse, sampler2D specular) {

	vec4 ambientColor, diffuseColor, specularColor;

	vec3 lightDir = normalize(-light.direction);

	// ambient
	ambientColor = vec4(light.ambient, 1.0) * texture(diffuse, texCoords);

	// diffuse
	float diffEff = max(dot(normal, lightDir), 0.0);
	diffuseColor = diffEff * vec4(light.diffuse, 1.0) * texture(diffuse, texCoords);

	// specular
	//vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	//float specEff = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
	float specEff = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	specularColor = specEff * vec4(light.specular, 1.0) * texture(specular, texCoords);

	float shadow = CalcParallelShadow(lightDir, normal);

	// result
	return ambientColor + (diffuseColor + specularColor) * (1.2 - shadow);
}

vec4 CalcPointLight(Point_Light_t light, vec3 normal, vec3 viewDir, vec3 fragPos,
	vec2 texCoords, sampler2D diffuse, sampler2D specular) {

	vec4 ambientColor, diffuseColor, specularColor;

	vec3 lightDir = normalize(light.position - fragPos);

	// Physics
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear*distance + light.quadratic*distance*distance);

	// ambient
	ambientColor = vec4(light.ambient, 1.0) * texture(diffuse, texCoords);

	// diffuse
	float diffEff = max(dot(normal, lightDir), 0.0);
	diffuseColor = diffEff * vec4(light.diffuse, 1.0) * texture(diffuse, texCoords);

	// specular
	//vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	//float specEff = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
	float specEff = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	specularColor = specEff * vec4(light.specular, 1.0) * texture(specular, texCoords);

	// result
	return attenuation * (ambientColor + diffuseColor + specularColor);
}

vec4 CalcSpotLight(Spot_Light_t light, vec3 normal, vec3 viewDir, vec3 fragPos,
	vec2 texCoords, sampler2D diffuse, sampler2D specular) {

	vec4 ambientColor, diffuseColor, specularColor;

	vec3 lightDir = normalize(light.position - fragPos);

	// Physics
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear*distance + light.quadratic*distance*distance);
	float theta = dot(lightDir, normalize(-light.direction)); // still in world coordinate
	float epsilon = light.innerCutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	// Ambient lighting
	ambientColor = vec4(light.ambient, 1.0) * texture(diffuse, texCoords);

	// Diffuse lighting
	float diffEff = max(dot(normal, lightDir), 0.0);
	diffuseColor = diffEff * vec4(light.diffuse, 1.0) * texture(diffuse, texCoords);

	// Specular lighting
	//vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	//float specEff = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
	float specEff = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	specularColor = specEff * vec4(light.specular, 1.0) * texture(specular, texCoords);

	// Result lighting
	return attenuation * (ambientColor + (diffuseColor + specularColor) * intensity);
}

vec4 CalcEmission(vec2 texCoords, sampler2D emission, vec4 testLight) {

	// emission
	vec4 emissionColor = texture(emission, texCoords);

	// Ignore black parts of emission texture, transparentize them
	if (emissionColor.x < 0.3 || emissionColor.y < 0.3 || emissionColor.z < 0.3)
		emissionColor = vec4(0.0, 0.0, 0.0, 0.0);

	// Turn off light if environment is bright enough (simulate city lighting)
	if (testLight.x > 0.1 || testLight.y > 0.1 || testLight.z > 0.1)
		emissionColor = vec4(0.0, 0.0, 0.0, 0.0);

	return emissionColor;
}
