#version 460 core

layout(location = 0) uniform mat4 view;
layout(location = 1) uniform mat4 projection;

layout(location = 2) uniform float scale;
layout(location = 3) uniform vec2 offset;

layout(location = 0) in vec3 position;

void main()
{
  vec2 xz = offset + vec4(position, 1.0).xz * scale;

  float y = 0.0;

  vec3 worldPosition = vec3(y);
  worldPosition.xz = xz;

	gl_Position = projection * view * vec4(worldPosition, 1.0);
}