#version 460 core

// Shader storage buffer with the positions of the centers of the particles
layout(std140, binding = 0) buffer ParticlesPositions
{
  // When using [], then the size of this array is determined at the time the shader
  // is executed. The size is the rest of this buffer object range
	vec2[] particlesPositions;
};

uniform float particleSize;

layout(location = 0) in vec2 billboardQuadVertex;

void main()
{
	vec2 vertexPosition = particlesPositions[gl_DrawID] + billboardQuadVertex * particleSize;

	gl_Position = vec4(vertexPosition, 0.0, 1.0);
}