#include "terrain_chunk_generator.h"

namespace Lotus
{

  ProceduralDataGenerator::ProceduralDataGenerator(
      uint16_t generatorDataPerChunkSide,
      uint16_t generatorChunksPerSide,
      Vec2i generatorDataOrigin,
      uint32_t generatorSeed) : 
    dataPerChunkSide(generatorDataPerChunkSide),
    chunksPerSide(generatorChunksPerSide),
    dataOrigin(generatorDataOrigin),
    chunksOrigin({ 0 , 0 })
  {
    chunksData.reserve(chunksPerSide * chunksPerSide);

    for (int i = 0; i < chunksPerSide * chunksPerSide; i++)
    {
      float* chunkData = new float[dataPerChunkSide * dataPerChunkSide];
      chunksData.push_back(chunkData);
    }

    for (int x = 0; x < chunksPerSide; x++)
    {
      for (int y = 0; y < chunksPerSide; y++)
      {
        generateChunkData(x, y, x, y);
      }
    }
  }

  ProceduralDataGenerator::~ProceduralDataGenerator()
  {
    for (int i = 0; i < chunksPerSide * chunksPerSide; i++)
    {
      delete chunksData[i];
    }
  }

  const float* ProceduralDataGenerator::getChunkData(int x, int y) const
  {
    return chunksData[y * chunksPerSide + x];
  }

  void ProceduralDataGenerator::updateUp()
  {
    dataOrigin.y -= dataPerChunkSide;
    chunksOrigin.y = (chunksOrigin.y + chunksPerSide - 1) % chunksPerSide;
    
    for (int x = 0; x < chunksPerSide; x++)
    {
      generateChunkData(x, getUp(), (x + chunksPerSide - getLeft()) % chunksPerSide, 0);
    }
  }

  void ProceduralDataGenerator::updateRight()
  {
    dataOrigin.x += dataPerChunkSide;
    chunksOrigin.x = (chunksOrigin.x + 1) % chunksPerSide;

    for (int y = 0; y < chunksPerSide; y++)
    {
      generateChunkData(getRight(), y, chunksPerSide - 1, (y + chunksPerSide - getUp()) % chunksPerSide);
    }
  }

  void ProceduralDataGenerator::updateDown()
  {
    dataOrigin.y += dataPerChunkSide;
    chunksOrigin.y = (chunksOrigin.y + 1) % chunksPerSide;

    for (int x = 0; x < chunksPerSide; x++)
    {
      generateChunkData(x, getDown(), (x + chunksPerSide - getLeft()) % chunksPerSide, chunksPerSide - 1);
    }
  }

  void ProceduralDataGenerator::updateLeft()
  {
    dataOrigin.x -= dataPerChunkSide;
    chunksOrigin.x = (chunksOrigin.x + chunksPerSide - 1) % chunksPerSide;
    
    for (int y = 0; y < chunksPerSide; y++)
    {
      generateChunkData(getLeft(), y, 0, (y + chunksPerSide - getUp()) % chunksPerSide);
    }
  }

  void ProceduralDataGenerator::generateChunkData(int x, int y, int worldX, int worldY)
  {
    int64_t offsetX = dataOrigin.x - static_cast<int64_t>((chunksPerSide * dataPerChunkSide) * 0.5) + worldX * dataPerChunkSide;
    int64_t offsetY = dataOrigin.y - static_cast<int64_t>((chunksPerSide * dataPerChunkSide) * 0.5) + worldY * dataPerChunkSide;

    glm::vec2 offset = { offsetX, offsetY };
    
    float* chunkData = chunksData[y * chunksPerSide + x];

    Perlin2DArray::fill(chunkData, dataPerChunkSide, dataPerChunkSide, offset);
  }

}