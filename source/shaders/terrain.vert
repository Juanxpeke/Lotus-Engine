#version 460 core

layout(location = 0) uniform mat4 view;
layout(location = 1) uniform mat4 projection;

layout(location = 0) in vec3 position;

void main()
{
	gl_Position = projection * view * vec4(position, 1.0);
}