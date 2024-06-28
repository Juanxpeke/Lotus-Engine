#version 460 core

#include ../common/lighting.glsl

// Lights information uniform
layout(std140, binding = 0) uniform Lights
{
	DirectionalLight[${MAX_DIRECTIONAL_LIGHTS}] directionalLights;
	PointLight[${MAX_POINT_LIGHTS}] pointLights;
	vec3 ambientLight;
	int directionalLightsCount;
	int pointLightsCount;
};

layout(location = 10) uniform vec3 debugColor;

/*
	Inputs
*/
in vec3 fragPosition;
in vec3 fragNormal;

/*
	Ouputs
*/
out vec4 outColor;

void main()
{
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

	vec3 result = (ambient + Lo) * vec3(1.0, 1.0, 1.0) + (debugColor * 0);

	outColor = vec4(result, 1.0);
}


