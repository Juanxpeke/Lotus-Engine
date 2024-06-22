#version 460 core

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 projection;
layout(location = 9) uniform vec3 cameraPosition;

layout(location = 3) uniform float levelScale;
layout(location = 4) uniform vec2 offset;

layout(location = 5) uniform sampler2DArray heightmaps;

// Inputs
layout(location = 0) in vec3 position;

void main()
{
  vec2 xz = offset + (model * vec4(position, 1.0)).xz * levelScale;
  
  ivec2 ixz = ivec2(xz);

  int dataPerSide = 256 * 4; 
  int dataPerHalfSide = dataPerSide / 2;

  ivec2 dataOrigin = ivec2(-512, -512);
  ivec2 dataEnd = ivec2(512, 512);

  ivec2 globalDataXZ = ixz - dataOrigin;

  int coordX = globalDataXZ.x % 256;
  int chunkX = globalDataXZ.x / 256;

  int coordY = globalDataXZ.y % 256;
  int chunkY = globalDataXZ.y / 256;

  int layer = chunkY * 4 + chunkX;

  float y = 64.0 * texelFetch(heightmaps, ivec3(coordX, coordY, layer), 0).r;

  if (globalDataXZ.x < 0 || globalDataXZ.y < 0 || globalDataXZ.x > dataPerSide || globalDataXZ.y > dataPerSide)
  {
    y = 0;
  }
  
  vec3 worldPosition = vec3(y);
  worldPosition.xz = xz;

	gl_Position = projection * view * vec4(worldPosition, 1.0);
}