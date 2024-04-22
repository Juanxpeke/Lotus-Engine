// All expressions of the form ${SOME_NAME} are replaced before runtime to compile this shader

#version 460 core

struct DirectionalLight
{
	vec3 color;
	vec3 direction;
};

struct PointLight
{
	vec3 color;
	vec3 position;
	float maxRadius;
};

struct SpotLight
{
	vec3 color; 
	float maxRadius;
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
layout (location = 4) uniform vec3 materialTint;

in vec3 fragNormal;
in vec3 fragPosition;
in vec2 fragTexCoord;

out vec4 outColor;


// Calcula del decaimiento de la intensidad luminica dada la distancia a ella
// lightVector corresponde un vector que apunta desde la superficie iluminada a la fuente de luz
// lightRadius es el radio de fuente de luz
float getDistanceAttenuation(vec3 lightVector, float lightRadius)
{
	float squareDistance = dot(lightVector, lightVector);
	float squareRadius = lightRadius * lightRadius;
	
	// El factor de windowing permite que esta funcion retorne 1 para distancia igual a 0 y 0 para distancia igual al rango de la luz

	// float windowing = pow(max(1.0 - pow(squareDistance / squareRadius, 2.0f), 0.0f), 2.0f);
	// float distanceAttenuation = windowing * (1 / (squareDistance + 1));
	
	float cdistance = length(lightVector);
  float attenuation = 1 / (cdistance * cdistance);
	
	
	
	return 1.0;
}

//Calcula del decaimiento de la intansidad luminica dada una diferencia angular a ella
//normalizedLightVector es una vector normalizado que apunta desde la fuente a la superficie iliminada
//lightDirection corresponde a la direccion de la fuente tipo spotlight
//Para angulos menores al angulo de umbra la funcion retorna 1
//Para angulos mayores al angulo de penumbra la funcion retorna 0
//Para el resto de los casos la funcion retorna valores entre 1 y 0.
float getAngularAttenuation(vec3 normalizedLightVector, vec3 lightDirection, float lightCosUmbraAngle, float lightCosPenumbraAngle)
{
	float cosSurfaceAngle = dot(lightDirection, normalizedLightVector);
	float t = clamp((cosSurfaceAngle - lightCosUmbraAngle) / (lightCosPenumbraAngle - lightCosUmbraAngle), 0.0f, 1.0f);
	float angularAttenuation = t*t;
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
		Lo += directionalLights[i].color * max(dot(normal, -directionalLights[i].direction), 0.0);
	}

	// Point lights iteration
	for(int i = 0; i < pointLightsCount; i++)
	{
		vec3 lightVector = fragPosition - pointLights[i].position;
		vec3 lightDirection = normalize(lightVector);
		//float distanceAttenuation = getDistanceAttenuation(lightVector, pointLights[i].maxRadius);
		float xdistance = length(lightVector);
    float attenuation = 1.0 / (1.0 + 0.25 * xdistance + 0.07 * (xdistance * xdistance));    
		Lo += 1.0 * pointLights[i].color * max(dot(normal, -lightDirection), 0.0);
	}
	
	vec3 diffuseColor = texture(diffuseTexture, fragTexCoord).xyz;

	vec3 result = (ambient + Lo) * diffuseColor;

	outColor = vec4(materialTint * result, 1.0);
}