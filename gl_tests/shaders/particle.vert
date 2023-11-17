#version 460 core

uniform float particleSize;
uniform vec2 particlePosition; // Position of the center of the particle

layout(location = 0) in vec2 billboardQuadVertex;

void main()
{
	vec2 vertexPosition = particlePosition + billboardQuadVertex * particleSize;

	gl_Position = vec4(vertexPosition, 0.0, 1.0);
}