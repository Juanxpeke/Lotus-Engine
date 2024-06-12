// All expressions of the form ${SOME_NAME} are replaced before runtime compile this shader

#version 460 core

#include common/lighting.glsl
#include common/primitives.glsl

struct Material
{
	vec3 diffuseColor;
	int int_0;
	vec3 vec3_1;
	int int_1;
	vec3 vec3_2;//sampler2D texture_0;
	int xd;//sampler2D texture_1;
	vec3 vec3_3;//sampler2D texture_2;
	int xdd;//sampler2D texture_3;
};

// Shader storage buffer with the objects
layout(std140, binding = 0) readonly buffer Objects
{
  // When using [], then the size of this array is determined at the time the shader
  // is executed. The size is the rest of this buffer object range
	Object[] objects;
};

// Shader storage buffer with the materials of each mesh instance
layout(std140, binding = 2) readonly buffer Materials
{
  // When using [], then the size of this array is determined at the time the shader
  // is executed. The size is the rest of this buffer object range
	Material[] materials;
};

// Lights information uniform
layout(std140, binding = 0) uniform Lights
{
	DirectionalLight[${MAX_DIRECTIONAL_LIGHTS}] directionalLights;
	PointLight[${MAX_POINT_LIGHTS}] pointLights;
	vec3 ambientLight;
	int directionalLightsCount;
	int pointLightsCount;
};

flat in uint fragObjectID;
in vec3 fragPosition;
in vec3 fragNormal;

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

	vec3 result = (ambient + Lo) * material.diffuseColor; 
	
	outColor = vec4(result, 1.0);
}


