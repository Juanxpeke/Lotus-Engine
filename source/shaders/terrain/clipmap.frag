#version 460 core

layout(location = 6) uniform vec3 debugColor;


out vec4 outColor;

void main()
{
	outColor = vec4(debugColor, 1.0);
}


