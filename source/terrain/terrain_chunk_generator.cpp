#include "terrain_chunk_generator.h"

namespace Lotus
{

  TerrainChunkGenerator::TerrainChunkGenerator(uint8_t terrainChunkSize) : 
      chunkSize(terrainChunkSize),
      originX(0),
      originY(0),
      topLeftChunkX(0),
      topLeftChunkY(0)
  {
    chunks2DArrays.fill(nullptr);

    for (int x = 0; x < chunksPerSide; x++)
    {
      for (int y = 0; y < chunksPerSide; y++)
      {
        generateChunk(x, y);
      }
    }
  } 

  void TerrainChunkGenerator::updateTop()
  {
    originY -= chunkSize;
    topLeftChunkY = (topLeftChunkY + chunksPerSide - 1) % chunksPerSide;
    
    for (int x = 0; x < chunksPerSide; x++)
    {
      generateChunk(x, topLeftChunkY);
    }
  }

  void TerrainChunkGenerator::updateRight()
  {
    originX += chunkSize;
    topLeftChunkX = (topLeftChunkX + 1) % chunksPerSide;

    for (int y = 0; y < chunksPerSide; y++)
    {
      generateChunk((topLeftChunkX + chunksPerSide - 1) % chunksPerSide, y);
    }
  }

  void TerrainChunkGenerator::updateDown()
  {
    originY += chunkSize;
    topLeftChunkY = (topLeftChunkY + 1) % chunksPerSide;

    for (int x = 0; x < chunksPerSide; x++)
    {
      generateChunk(x, (topLeftChunkY + chunksPerSide - 1) % chunksPerSide);
    }
  }

  void TerrainChunkGenerator::updateLeft()
  {
    originX -= chunkSize;
    topLeftChunkX = (topLeftChunkX + chunksPerSide - 1) % chunksPerSide;
    
    for (int y = 0; y < chunksPerSide; y++)
    {
      generateChunk(topLeftChunkX, y);
    }
  }

  void TerrainChunkGenerator::generateChunk(int x, int y)
  {
    int64_t offsetX = originX - static_cast<int64_t>((chunksPerSide * chunkSize) * 0.5) + x * chunkSize;
    int64_t offsetY = originY - static_cast<int64_t>((chunksPerSide * chunkSize) * 0.5) + y * chunkSize;

    glm::vec2 offset = { offsetX, offsetY };

    Perlin2DArray* perlin2DArray = new Perlin2DArray(chunkSize, chunkSize, offset);
    Perlin2DArray* oldPerlin2DArray = chunks2DArrays[y * chunksPerSide + x];
    
    chunks2DArrays[y * chunksPerSide + x] = perlin2DArray;
    
    if (oldPerlin2DArray)
    {
      delete oldPerlin2DArray;
    }
  }

}