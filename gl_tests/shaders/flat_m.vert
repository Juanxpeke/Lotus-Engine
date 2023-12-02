/*
  Flat shader for drawing instances in different positions. It uses a uniform
  buffer object with the models and a draw index to setup per instance models
*/

#version 460 core

// Models uniform buffer
layout(std140, binding = 0) uniform Models
{
  // For me, UBO maximum size is 64Kib (with std140 layout each element in an array
  // of mat4 occupies 64 bytes)
	mat4[1024] models;
};

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;

out vec3 fragColor;

void main(void)
{
  fragColor = color;
  gl_Position = models[gl_DrawID] * vec4(position, 0.0, 1.0);
}