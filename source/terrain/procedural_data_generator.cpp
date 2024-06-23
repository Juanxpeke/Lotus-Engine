#include "procedural_data_generator.h"

#include "../util/log.h"

namespace Lotus
{

  ProceduralDataGenerator::ProceduralDataGenerator(
      uint16_t generatorDataPerChunkSide,
      uint16_t generatorChunksPerSide,
      const PerlinNoiseConfig& generatorNoiseConfig,
      const Vec2i& generatorDataOrigin) : 
    dataPerChunkSide(generatorDataPerChunkSide),
    chunksPerSide(generatorChunksPerSide),
    dataOrigin(generatorDataOrigin),
    chunksOrigin({ 0 , 0 }),
    noiseConfig(generatorNoiseConfig)
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
        generateChunkData(x, y);
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

  const float* ProceduralDataGenerator::getChunkData(const Vec2i& chunk) const
  {
    return getChunkData(chunk.x, chunk.y);
  }

  const float* ProceduralDataGenerator::getChunkData(int x, int y) const
  {
    return chunksData[y * chunksPerSide + x];
  }

  void ProceduralDataGenerator::updateTopChunks()
  {
    dataOrigin.y -= dataPerChunkSide;
    chunksOrigin.y = (chunksOrigin.y + chunksPerSide - 1) % chunksPerSide;
    
    for (int x = 0; x < chunksPerSide; x++)
    {
      generateChunkData(x, getChunksTop());
    }

    LOTUS_LOG_INFO("[Procedural Data Generator Log] Updated top chunks");
  }

  void ProceduralDataGenerator::updateRightChunks()
  {
    dataOrigin.x += dataPerChunkSide;
    chunksOrigin.x = (chunksOrigin.x + 1) % chunksPerSide;

    for (int y = 0; y < chunksPerSide; y++)
    {
      generateChunkData(getChunksRight(), y);
    }

    LOTUS_LOG_INFO("[Procedural Data Generator Log] Updated right chunks");
  }

  void ProceduralDataGenerator::updateBottomChunks()
  {
    dataOrigin.y += dataPerChunkSide;
    chunksOrigin.y = (chunksOrigin.y + 1) % chunksPerSide;

    for (int x = 0; x < chunksPerSide; x++)
    {
      generateChunkData(x, getChunksBottom());
    }

    LOTUS_LOG_INFO("[Procedural Data Generator Log] Updated bottom chunks");
  }

  void ProceduralDataGenerator::updateLeftChunks()
  {
    dataOrigin.x -= dataPerChunkSide;
    chunksOrigin.x = (chunksOrigin.x + chunksPerSide - 1) % chunksPerSide;
    
    for (int y = 0; y < chunksPerSide; y++)
    {
      generateChunkData(getChunksLeft(), y);
    }

    LOTUS_LOG_INFO("[Procedural Data Generator Log] Updated left chunks");
  }

  void ProceduralDataGenerator::generateChunkData(const Vec2i& chunk)
  {
    generateChunkData(chunk.x, chunk.y);
  }

  void ProceduralDataGenerator::generateChunkData(int x, int y)
  {
    Vec2i dataChunk((x - getChunksLeft() + chunksPerSide) % chunksPerSide, (y - getChunksTop() + chunksPerSide) % chunksPerSide);

    Vec2i offset = dataOrigin - Vec2i((chunksPerSide * dataPerChunkSide) / 2) + dataChunk * dataPerChunkSide;
    
    float* chunkData = chunksData[y * chunksPerSide + x];

    noiseConfig.offset = offset;

    Perlin2DArray::fill(chunkData, dataPerChunkSide, dataPerChunkSide, noiseConfig);
  }

}