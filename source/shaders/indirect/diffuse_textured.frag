// All expressions of the form ${SOME_NAME} are replaced before runtime compile this shader

#version 460 core

// Enable bindless textures
#extension GL_ARB_bindless_texture : require

struct Material
{
	vec3 diffuseTextureTint;
	int int_0;
	vec3 vec3_1;
	int int_1;
	sampler2D diffuseTexture;
	sampler2D tex_1;
	sampler2D tex_2;
	sampler2D tex_3;
};

// Shader storage buffer with the materials
layout(std140, binding = 2) readonly buffer Materials
{
	Material[] materials;
};

// Textures block
layout (std140, binding = 1) uniform Textures
{
  sampler2D textures[512];
};

// Lights information uniform
layout(std140, binding = 1) uniform Lights
{
	DirectionalLight[${MAX_DIRECTIONAL_LIGHTS}] directionalLights;
	PointLight[${MAX_POINT_LIGHTS}] pointLights;
	vec3 ambientLight;
	int directionalLightsCount;
	int pointLightsCount;
};

// Inputs
flat in uint fragObjectID;
in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

// Outputs
out vec4 outColor;

void main()
{
	Object object = objects[fragObjectID];
	Material material = materials[object.materialHandle];

	vec3 ambient = ambientLight;

	vec3 normal = normalize(fragNormal);

	// Light contribution accumulated value from all light sources
	vec3 Lo = vec3(0.0f, 0.0f, 0.0f);
	
	// Directional lights iteration
	for(int i = 0; i < directionalLightsCount; i++)
	{
		Lo += directionalLights[i].colorIntensity * max(dot(normal, -directionalLights[i].direction), 0.0);
	}

	// Point lights iteration
	for(int i = 0; i < pointLightsCount; i++)
	{
		vec3 lightVector = fragPosition - pointLights[i].position;
		vec3 lightDirection = normalize(lightVector);
		float distanceAttenuation = getDistanceAttenuation(lightVector, pointLights[i].radius);
		Lo += distanceAttenuation * pointLights[i].colorIntensity * max(dot(normal, -lightDirection), 0.0);
	}
	
	vec3 diffuseColor = texture(material.diffuseTexture, fragTexCoord).xyz;

	vec3 result = (ambient + Lo) * diffuseColor;

	outColor = vec4(material.diffuseTextureTint * result, 1.0);
}