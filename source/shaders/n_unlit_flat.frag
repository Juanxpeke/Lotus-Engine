// All expressions of the form ${SOME_NAME} are replaced before runtime compile this shader

#version 460 core

struct Object
{
  mat4 model;
  uint materialHandle;
};

struct Material
{
	vec3 unlitColor;
	int int_0;
	vec3 vec3_1;
	int int_1;
	vec3 vec3_2;//sampler2D texture_0;
	int xd;//sampler2D texture_1;
	vec3 vec3_3;//sampler2D texture_2;
	int xdd;//sampler2D texture_3;
};

// Shader storage buffer with the objects
layout(std140, binding = 0) readonly buffer Objects
{
  // When using [], then the size of this array is determined at the time the shader
  // is executed. The size is the rest of this buffer object range
	Object[] objects;
};

// Shader storage buffer with the materials of each mesh instance
layout(std140, binding = 2) readonly buffer Materials
{
  // When using [], then the size of this array is determined at the time the shader
  // is executed. The size is the rest of this buffer object range
	Material[] materials;
};

flat in uint fragObjectID;

out vec4 outColor;


void main()
{
	Object object = objects[fragObjectID];
	Material material = materials[object.materialHandle];

	outColor = vec4(material.unlitColor, 1.0);
}