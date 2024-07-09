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
layout(location = 1) in vec3 normal;

out vec3 fragPosition;
out vec3 fragNormal;

void main()
{
	fragPosition = vec3(model * vec4(position, 1.0));
	fragNormal = mat3(modelInverseTranspose) * normal;
	
	gl_Position = viewProjection * model * vec4(position, 1.0);
}