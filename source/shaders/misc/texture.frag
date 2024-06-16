#version 460 core

layout(location = 3) uniform sampler2D quadTexture;

in vec2 fragTexCoord;

out vec4 outColor;

void main()
{
	vec3 color = texture(quadTexture, fragTexCoord).xyz;

	outColor = vec4(color, 1.0);
}