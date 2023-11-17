/*
  Flat shader for drawing instances in different positions. It uses a uniform
  buffer object with the models and a instance index to setup per instance models
*/

#version 460 core

// Models uniform buffer
layout(std140, binding = 0) uniform Models
{
	mat4[256] models;
};


layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;

out vec3 fragColor;

void main(void)
{
  fragColor = color;
  gl_Position = models[gl_InstanceID] * vec4(position, 0.0, 1.0);
}