#version 460 core

#include ../common/primitives.glsl

// Shader storage buffer with the objects
layout(std140, binding = 0) readonly buffer Objects
{
	Object[] objects;
};

// Shader storage buffer with the objects
layout(std430, binding = 1) readonly buffer ObjectHandles
{
	uint[] objectHandles;
};

layout(location = 0) uniform mat4 view;
layout(location = 1) uniform mat4 projection;

// Inputs
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

// Outputs
flat out uint fragObjectID;
out vec3 fragPosition;
out vec3 fragNormal;

void main()
{
	uint objectID = objectHandles[gl_BaseInstance + gl_InstanceID];

  Object object = objects[objectID];

	fragObjectID = objectID;
	fragPosition = vec3(object.model * vec4(position, 1.0));
	fragNormal = mat3(transpose(inverse(object.model))) * normal;
	
	gl_Position = projection * view * object.model * vec4(position, 1.0);
}