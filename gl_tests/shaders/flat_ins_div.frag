/*
  Flat shader for drawing instances in different positions. It uses vertex
  attributes and the vertex attrib divisor to setup per instance matrices
*/

#version 460 core

layout (location = 0) in vec3 fragColor;
layout (location = 0) out vec4 outColor;

void main(void)
{
  outColor = vec4(fragColor, 1.0);
}