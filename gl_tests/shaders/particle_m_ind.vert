#version 460 core

// Uniform buffer with the positions of the centers of the particles
layout(std140, binding = 0) uniform ParticlesPositions
{
  // For me, UBO maximum size is 64Kib (with std140 layout each element in an array
	// of vectors occupies the same as a vec4, that is, 16 bytes)  
	vec2[4096] particlesPositions;
};

uniform float particleSize;

layout(location = 0) in vec2 billboardQuadVertex;

void main()
{
	vec2 vertexPosition = particlesPositions[gl_DrawID] + billboardQuadVertex * particleSize;

	gl_Position = vec4(vertexPosition, 0.0, 1.0);
}