#include "terrain_chunk_generator.h"

namespace Lotus
{

  TerrainChunkGenerator::TerrainChunkGenerator(uint16_t terrainChunkSize) : 
      chunkSize(terrainChunkSize),
      dataOriginX(0),
      dataOriginY(0),
      left(0),
      up(0)
  {
    chunks2DArrays.fill(nullptr);

    for (int x = 0; x < ChunksPerSide; x++)
    {
      for (int y = 0; y < ChunksPerSide; y++)
      {
        generateChunk(x, y, x, y);
      }
    }
  }

  const float* TerrainChunkGenerator::getChunkData(int x, int y) const
  {
    return (chunks2DArrays[y * ChunksPerSide + x])->getData();
  }

  void TerrainChunkGenerator::updateUp()
  {
    dataOriginY -= chunkSize;
    up = (up + ChunksPerSide - 1) % ChunksPerSide;
    
    for (int x = 0; x < ChunksPerSide; x++)
    {
      generateChunk(x, getUp(), (x + ChunksPerSide - getLeft()) % ChunksPerSide, 0);
    }
  }

  void TerrainChunkGenerator::updateRight()
  {
    dataOriginX += chunkSize;
    left = (left + 1) % ChunksPerSide;

    for (int y = 0; y < ChunksPerSide; y++)
    {
      generateChunk(getRight(), y, ChunksPerSide - 1, (y + ChunksPerSide - getUp()) % ChunksPerSide);
    }
  }

  void TerrainChunkGenerator::updateDown()
  {
    dataOriginY += chunkSize;
    up = (up + 1) % ChunksPerSide;

    for (int x = 0; x < ChunksPerSide; x++)
    {
      generateChunk(x, getDown(), (x + ChunksPerSide - getLeft()) % ChunksPerSide, ChunksPerSide - 1);
    }
  }

  void TerrainChunkGenerator::updateLeft()
  {
    dataOriginX -= chunkSize;
    left = (left + ChunksPerSide - 1) % ChunksPerSide;
    
    for (int y = 0; y < ChunksPerSide; y++)
    {
      generateChunk(getLeft(), y, 0, (y + ChunksPerSide - getUp()) % ChunksPerSide);
    }
  }

  void TerrainChunkGenerator::generateChunk(int x, int y, int worldX, int worldY)
  {
    int64_t offsetX = dataOriginX - static_cast<int64_t>((ChunksPerSide * chunkSize) * 0.5) + worldX * chunkSize;
    int64_t offsetY = dataOriginY - static_cast<int64_t>((ChunksPerSide * chunkSize) * 0.5) + worldY * chunkSize;

    glm::vec2 offset = { offsetX, offsetY };

    Perlin2DArray* perlin2DArray = new Perlin2DArray(chunkSize, chunkSize, offset);
    Perlin2DArray* oldPerlin2DArray = chunks2DArrays[y * ChunksPerSide + x];
    
    chunks2DArrays[y * ChunksPerSide + x] = perlin2DArray;
    
    if (oldPerlin2DArray)
    {
      delete oldPerlin2DArray;
    }
  }

}