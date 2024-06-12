#version 460 core

struct Object
{
  mat4 model;
  uint materialHandle;
};

// Shader storage buffer with the objects
layout(std140, binding = 0) readonly buffer Objects
{
  // When using [], then the size of this array is determined at the time the shader
  // is executed. The size is the rest of this buffer object range
	Object[] objects;
};

// Shader storage buffer with the objects
layout(std430, binding = 1) readonly buffer ObjectHandles
{
  // When using [], then the size of this array is determined at the time the shader
  // is executed. The size is the rest of this buffer object range
	uint[] objectHandles;
};

layout(location = 0) uniform mat4 view;
layout(location = 1) uniform mat4 projection;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

flat out uint fragObjectID;


void main()
{
  uint objectID = objectHandles[gl_BaseInstance + gl_InstanceID];

  Object object = objects[objectID];

	fragObjectID = objectID;

	gl_Position = projection * view * object.model * vec4(position, 1.0);
}