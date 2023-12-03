#version 460 core

// Shader storage buffer with the positions of the centers of the particles
layout(std140, binding = 0) buffer ParticlesPositions
{
  // When using [], then the size of this array is determined at the time the shader
  // is executed. The size is the rest of this buffer object range
	vec2[] particlesPositions;
};

uniform float particleSize;

layout(location = 0) in vec2 billboardVertex;

void main()
{
	// As we only need each particle to have a different position, we can leave them
	// interleaved by shape (quad = 0 3 6 ... | triangle = 1 4 7 ... | rect = 2 5 8 ...)
	// In the case we needed the positions divided by shape sequentially, that is, like
	// (quad = 0 1 2 ... | triangle = n_quads n_quads+1 n_quads+2 ...), we could use 
	// uniforms to know the amount of instances for each shape
	int positionID = gl_InstanceID * 2 + gl_DrawID;
	vec2 vertexPosition = particlesPositions[positionID] + billboardVertex * particleSize;

	gl_Position = vec4(vertexPosition, 0.0, 1.0);
}