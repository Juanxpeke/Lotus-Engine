// All expressions of the form ${SOME_NAME} are replaced before runtime to compile this shader

#version 460 core

struct DirectionalLight
{
	vec3 color;
	vec3 direction;
};

// Lights information uniform
layout(std140, binding = 0) uniform Lights
{
	DirectionalLight[${MAX_DIRECTIONAL_LIGHTS}] directionalLights;
	vec3 ambientLight;
	int directionalLightsCount;
};

layout (location = 3) uniform vec3 diffuseColor;

in vec3 fragNormal;
in vec3 fragPosition;

out vec4 outColor;

void main()
{
	vec3 ambient = ambientLight;

	// Light contribution accumulated value from all light sources
	vec3 Lo = vec3(0.0f, 0.0f, 0.0f);

	// Directional lights iteration
	for(int i = 0; i < directionalLightsCount; i++)
	{
		Lo += directionalLights[i].color * max(dot(fragNormal, -directionalLights[i].direction), 0.0);
	}

	vec3 result = (ambient + Lo) * diffuseColor;

	outColor = vec4(result, 1.0);
}


