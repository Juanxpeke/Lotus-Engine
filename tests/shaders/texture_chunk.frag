#version 460 core

layout(location = 3) uniform sampler2DArray sampleTextureArray;
layout(location = 4) uniform float userOffsetX;
layout(location = 5) uniform float userOffsetY;
layout(location = 6) uniform int left;
layout(location = 7) uniform int up;
 
in vec2 fragTexCoord;

out vec4 outColor;

int cmod(float x, int N)
{
  return int(mod(mod(x, N) + N, N));
}

void main()
{
  int offsetX = 1;
  int offsetY = 1;

  int sizeX = 2;
  int sizeY = 2;
  
  // [0 : 1) * 2 + 0.41 -> [0 : 2) + 0.41 -> [0.41 : 2.41)
  int chunkX = cmod(fragTexCoord.x * sizeX + userOffsetX + offsetX, 4); // int(fragTexCoord.x * sizeX + userOffsetX + offsetX) % 4;
  int chunkY = cmod(fragTexCoord.y * sizeY + userOffsetY + offsetY, 4);// int(fragTexCoord.y * sizeY + userOffsetY + offsetY) % 4;

  int layer = chunkY * 4 + chunkX;
  
  float xCoord = (fragTexCoord.x * sizeX + userOffsetX) - int(fragTexCoord.x * sizeX + userOffsetX);
  float yCoord = (fragTexCoord.y * sizeY + userOffsetY) - int(fragTexCoord.y * sizeY + userOffsetY);

	outColor = texture(sampleTextureArray, vec3(xCoord, yCoord, layer));
}