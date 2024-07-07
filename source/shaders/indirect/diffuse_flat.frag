// All expressions of the form ${SOME_NAME} are replaced before runtime compile this shader

#version 460 core

#include ../common/lighting.glsl
#include ../common/primitives.glsl

struct Material
{
	vec3 diffuseColor;
	int int_0;
	vec3 vec3_1;
	int int_1;
	vec3 vec3_2;
	int int_2;
	vec3 vec3_3;
	int int_3;
};

// Shader storage buffer with the objects
layout(std140, binding = 0) readonly buffer Objects
{
	Object[] objects;
};

// Shader storage buffer with the materials
layout(std140, binding = 2) readonly buffer Materials
{
	Material[] materials;
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

	vec3 result = (ambient + Lo) * material.diffuseColor; 
	
	outColor = vec4(result, 1.0);
}


