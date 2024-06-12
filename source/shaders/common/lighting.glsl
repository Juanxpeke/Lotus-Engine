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