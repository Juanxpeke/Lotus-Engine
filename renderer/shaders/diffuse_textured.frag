#version 460 core

//Es importante notar que todas expresiones de la forma ${SOME_NAME} son reemplazadas antes de compilar
layout (location = 3) uniform sampler2D diffuseTexture;
layout (location = 4) uniform vec3 materialTint;
out vec4 color;

in vec3 normal;
in vec3 worldPos;
in vec2 texCoord;

struct DirectionalLight {
	vec3 colorIntensity;
	vec3 direction;
};

struct PointLight {
	vec3 colorIntensity;
	vec3 position;
	float maxRadius;
};

struct SpotLight {
	vec3 colorIntensity; 
	float maxRadius;
	vec3 position; 
	float cosPenumbraAngle;
	vec3 direction;
	float cosUmbraAngle;
};

//Uniforme que contiene toda la información lumínica de la escena
layout(std140, binding = 0) uniform Lights
{
	DirectionalLight[${MAX_DIRECTIONAL_LIGHTS}] directionalLights;
	vec3 ambientLight;
	int directionalLightsCount;
};

//Calcula del decaimiento de la intensidad luminica dada la distancia a ella
// lightVector corresponde un vector que apunta desde la superficie iluminada a la fuente de luz
// lightRadius es el radio de fuente de luz
float GetDistanceAttenuation(vec3 lightVector, float lightRadius)
{
	float squareDistance = dot(lightVector, lightVector);
	float squareRadius = lightRadius * lightRadius;
	//El factor de windowing permite que esta funcion retorne 1 para distancia igual a 0 y 0 para distancia igual al rango de la luz
	float windowing = pow(max(1.0 - pow(squareDistance/squareRadius,2.0f),0.0f),2.0f);
	float distanceAttenuation = windowing * (1 / (squareDistance + 1));
	return distanceAttenuation;
}

//Calcula del decaimiento de la intansidad luminica dada una diferencia angular a ella
//normalizedLightVector es una vector normalizado que apunta desde la fuente a la superficie iliminada
//lightDirection corresponde a la direccion de la fuente tipo spotlight
//Para angulos menores al angulo de umbra la funcion retorna 1
//Para angulos mayores al angulo de penumbra la funcion retorna 0
//Para el resto de los casos la funcion retorna valores entre 1 y 0.
float GetAngularAttenuation(vec3 normalizedLightVector, vec3 lightDirection, float lightCosUmbraAngle, float lightCosPenumbraAngle)
{
	float cosSurfaceAngle = dot(lightDirection, normalizedLightVector);
	float t = clamp((cosSurfaceAngle - lightCosUmbraAngle) / (lightCosPenumbraAngle - lightCosUmbraAngle), 0.0f, 1.0f);
	float angularAttenuation = t*t;
	return angularAttenuation;
}


void main()
{
	vec3 ambient = ambientLight;
	vec3 norm = normalize(normal);
	vec3 Lo = vec3(0.0f,0.0f,0.0f);
	//Valor que acumulara el aporte lumínica de cada luz
	
	//Iteracion sobre luces direccionales
	for(int i = 0; i < directionalLightsCount; i++){
		Lo += directionalLights[i].colorIntensity * max(dot(norm, -directionalLights[i].direction), 0.0);
	}
	
	vec3 diffuseColor = texture(diffuseTexture, texCoord).xyz;
	vec3 result = (ambient + Lo) * diffuseColor;
	color = vec4(materialTint * result, 1.0);

}