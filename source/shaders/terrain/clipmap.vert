#version 460 core

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 projection;

/*
  Chunk generator variables
*/
layout(location = 3) uniform int dataPerChunkSide; // This should be the texture width and height
layout(location = 4) uniform int chunksPerSide;    // Layers in texture array should be chunksPerside to the square

layout(location = 5) uniform ivec2 chunksDataOrigin;
layout(location = 6) uniform ivec2 chunksOrigin;

/*
  Clipmap variables
*/
layout(location = 7) uniform float levelScale;
layout(location = 8) uniform vec2 offset;

layout(location = 9) uniform sampler2DArray heightmaps;

// Inputs
layout(location = 0) in vec3 position;

void main()
{
  vec2 xz = offset + (model * vec4(position, 1.0)).xz * levelScale;

  int dataPerSide = dataPerChunkSide * chunksPerSide; 
  int dataPerHalfSide = dataPerSide / 2;

  ivec2 dataOrigin = chunksDataOrigin - ivec2(dataPerHalfSide, dataPerHalfSide);

  ivec2 dataCoord = ivec2(xz) - dataOrigin;

  int texCoordX = dataCoord.x % dataPerChunkSide;
  int texCoordY = dataCoord.y % dataPerChunkSide;

  int chunkX = (dataCoord.x / dataPerChunkSide + chunksOrigin.x) % chunksPerSide;
  int chunkY = (dataCoord.y / dataPerChunkSide + chunksOrigin.y) % chunksPerSide;
  int layer = chunkY * chunksPerSide + chunkX;

  float y = 64.0 * texelFetch(heightmaps, ivec3(texCoordX, texCoordY, layer), 0).r;

  if (dataCoord.x < 0 || dataCoord.y < 0 || dataCoord.x > dataPerSide || dataCoord.y > dataPerSide)
  {
    y = 0;
  }

  vec3 worldPosition = vec3(y);
  worldPosition.xz = xz;

	gl_Position = projection * view * vec4(worldPosition, 1.0);
}