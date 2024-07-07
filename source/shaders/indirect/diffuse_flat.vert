#version 460 core

#include ../common/primitives.glsl

// Shader storage buffer with the objects
layout(std140, binding = 0) readonly buffer Objects
{
	Object[] objects;
};

// Shader storage buffer with the objects handles
layout(std430, binding = 1) readonly buffer ObjectHandles
{
	uint[] objectHandles;
};

layout(std140, binding = 0) uniform CameraBuffer
{
  mat4 view;
  mat4 projection;
  mat4 viewProjection;
  vec3 cameraPosition;
};

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