/*
  Flat shader for drawing instances in different positions. It uses vertex
  attributes and the vertex attrib divisor to setup per instance matrices
*/

#version 460 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;
layout (location = 3) in mat4 instanceMatrix;

layout (location = 0) out vec3 fragColor;

void main(void)
{
  fragColor = color;
  gl_Position = instanceMatrix * vec4(position, 0.0, 1.0);
}