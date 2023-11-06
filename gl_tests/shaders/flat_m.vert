/*
  Flat shader for drawing instances in different positions. It uses vertex
  attributes and the vertex attrib divisor to setup per instance matrices
*/

#version 460 core

// Model information uniform
layout(std140, binding = 0) uniform DrawMatrices
{
	mat4[256] drawMatrices;
};


layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;

layout (location = 0) out vec3 fragColor;

void main(void)
{
  fragColor = color;
  gl_Position =  drawMatrices[gl_DrawID] * vec4(position, 0.0, 1.0);
}