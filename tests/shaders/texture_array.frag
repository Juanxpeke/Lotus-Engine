#version 460 core

layout(location = 3) uniform sampler2DArray sampleTextureArray;

in vec2 fragTexCoord;

out vec4 outColor;

void main()
{
  int layer = int(fragTexCoord.x * 4);
  float xCoord = (fragTexCoord.x * 4) - layer;
  float yCoord = fragTexCoord.y;

	outColor = texture(sampleTextureArray, vec3(xCoord, yCoord, layer));
}