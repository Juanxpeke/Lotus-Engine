// All expressions of the form ${SOME_NAME} are replaced before runtime to compile this shader

#version 460 core

struct DirectionalLight
{
	vec3 colorIntensity;
	vec3 direction;
};

struct PointLight
{
	vec3 colorIntensity;
	vec3 position;
	float radius;
};

struct SpotLight
{
	vec3 colorIntensity; 
	float radius;
	vec3 position; 
	float cosPenumbraAngle;
	vec3 direction;
	float cosUmbraAngle;
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

layout (location = 3) uniform sampler2D diffuseTexture;
layout (location = 4) uniform vec3 diffuseTextureTint;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 outColor;

// Returns the light attenuation at a radial distance
// lightVector: Vector that goes from the illuminated surface to the light source
// lightRadius: Light source radius
// Based on UE4's light attenuation model (https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf)
float getDistanceAttenuation(vec3 lightVector, float lightRadius)
{
	float squareDistance = dot(lightVector, lightVector);
	float squareRadius = lightRadius * lightRadius;
	
	// The windowing factor returns 1 when distance is 0 and 0 when is equal to the light radius
	float windowing = pow(max(1.0 - pow(squareDistance / squareRadius, 2.0f), 0.0f), 2.0f);
	float distanceAttenuation = windowing * (1 / (squareDistance + 1));

	return distanceAttenuation;
}

// Returns the light attenuation given an angular difference between two vectors
// normalizedLightVector: Normalized vector that goes from the light source to the illuminated surface
// lightDirection: Spotlight direction
// If the angular difference is less than umbra angle, it returns 1
// If the angular difference is greater then the penumbra angle, it returns 0
// Otherwise, it returns values between 1 and 0
float getAngularAttenuation(vec3 normalizedLightVector, vec3 lightDirection, float lightCosUmbraAngle, float lightCosPenumbraAngle)
{
	float cosSurfaceAngle = dot(lightDirection, normalizedLightVector);
	float t = clamp((cosSurfaceAngle - lightCosUmbraAngle) / (lightCosPenumbraAngle - lightCosUmbraAngle), 0.0f, 1.0f);
	float angularAttenuation = t * t;
	return angularAttenuation;
}


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
	
	vec3 diffuseColor = texture(diffuseTexture, fragTexCoord).xyz;

	vec3 result = (ambient + Lo) * diffuseColor;

	outColor = vec4(diffuseTextureTint * result, 1.0);
}