#version 460 core

#include ../common/primitives.glsl

struct Material
{
	vec3 unlitColor;
	int int_0;
	vec3 vec3_1;
	int int_1;
	vec3 vec3_2;
	int int_2;
	vec3 vec3_3;
	int int_3;
};

// Shader storage buffer with the objects
layout(std140, binding = 0) readonly buffer Objects
{
	Object[] objects;
};

// Shader storage buffer with the materials
layout(std140, binding = 2) readonly buffer Materials
{
	Material[] materials;
};

// Inputs
flat in uint fragObjectID;

// Outputs
out vec4 outColor;

void main()
{
	Object object = objects[fragObjectID];
	Material material = materials[object.materialHandle];

	outColor = vec4(material.unlitColor, 1.0);
}