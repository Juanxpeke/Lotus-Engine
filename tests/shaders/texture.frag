#version 460 core

layout(location = 3) uniform sampler2D sampleTexture;

in vec2 fragTexCoord;

out vec4 outColor;

void main()
{
	outColor = texture(sampleTexture, fragTexCoord);
}