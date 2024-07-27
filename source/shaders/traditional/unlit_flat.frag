#version 460 core

layout(location = 3) uniform vec3 unlitColor;

out vec4 outColor;

void main()
{
	outColor = vec4(unlitColor, 1.0);
}