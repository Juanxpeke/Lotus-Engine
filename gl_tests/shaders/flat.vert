/*
  Flat shader for drawing instances in different positions. It uses a model
  uniform modified before each draw call according to each instance model
*/

#version 460 core

// Model uniform
uniform mat4 model;

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;

layout (location = 0) out vec3 fragColor;

void main(void)
{
  fragColor = color;
  gl_Position =  model * vec4(position, 0.0, 1.0);
}