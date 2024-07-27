#version 460 core

layout(std140, binding = 0) uniform CameraBuffer
{
  mat4 view;
  mat4 projection;
  mat4 viewProjection;
  vec3 cameraPosition;
};

layout(location = 1) uniform mat4 model;
layout(location = 2) uniform mat4 modelInverseTranspose;

layout(location = 0) in vec3 position;

void main()
{
	gl_Position = viewProjection * model * vec4(position, 1.0);
}