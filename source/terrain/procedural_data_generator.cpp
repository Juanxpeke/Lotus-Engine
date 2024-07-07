#include "procedural_data_generator.h"

#include <cmath>
#include "../util/log.h"

namespace Lotus
{
  constexpr char UnchangedFlag    = 0;
  constexpr char LoadedTopFlag    = 0b00001;
  constexpr char LoadedRightFlag  = 0b00010;
  constexpr char LoadedBottomFlag = 0b00100;
  constexpr char LoadedLeftFlag   = 0b01000;
  constexpr char ReloadedFlag     = 0b10000;

  ProceduralDataGenerator::ProceduralDataGenerator(
      uint16_t generatorDataPerChunkSide,
      uint8_t generatorChunksPerSide,
      const PerlinNoiseConfig& generatorNoiseConfig,
      const glm::vec2& initialObserverPosition) : 
    dataPerChunkSide(generatorDataPerChunkSide),
    chunksPerSide(generatorChunksPerSide),
    noiseConfig(generatorNoiseConfig)
  {
    chunksData.reserve(chunksPerSide * chunksPerSide);

    for (int i = 0; i < chunksPerSide * chunksPerSide; i++)
    {
      float* chunkData = new float[dataPerChunkSide * dataPerChunkSide];
      chunksData.push_back(chunkData);
    }

    reload(initialObserverPosition);
  }

  ProceduralDataGenerator::~ProceduralDataGenerator()
  {
    for (uint16_t i = 0; i < chunksPerSide * chunksPerSide; i++)
    {
      delete[] chunksData[i];
    }
  }

  const float* ProceduralDataGenerator::getChunkData(const glm::uvec2& chunk) const
  {
    return getChunkData(chunk.x, chunk.y);
  }

  const float* ProceduralDataGenerator::getChunkData(uint8_t x, uint8_t y) const
  {
    return chunksData[y * chunksPerSide + x];
  }

  bool ProceduralDataGenerator::updatedSincePreviousFrame(ProceduralUpdateRegion region) const
  {
    switch (region)
    {
      case ProceduralUpdateRegion::TopChunks:
        return static_cast<bool>(stateSincePreviousFrame & LoadedTopFlag);
      case ProceduralUpdateRegion::RightChunks:
        return static_cast<bool>(stateSincePreviousFrame & LoadedRightFlag);
      case ProceduralUpdateRegion::BottomChunks:
        return static_cast<bool>(stateSincePreviousFrame & LoadedBottomFlag);
      case ProceduralUpdateRegion::LeftChunks:
        return static_cast<bool>(stateSincePreviousFrame & LoadedLeftFlag);
      case ProceduralUpdateRegion::Everything:
        return static_cast<bool>(stateSincePreviousFrame & ReloadedFlag);
      default:
        return false;
    }
  }

  void ProceduralDataGenerator::registerObserverPosition(const glm::vec2& observerPosition)
  {
    glm::vec2 difference;

    difference.x = observerPosition.x - dataOrigin.x;
    difference.y = observerPosition.y - dataOrigin.y;

    stateSincePreviousFrame = UnchangedFlag;

    if (std::fabsf(difference.x) > 2 * dataPerChunkSide || std::fabsf(difference.y) > 2 * dataPerChunkSide)
    {
      reload(observerPosition);
      return;
    }

    if (difference.x > dataPerChunkSide)
    {
      loadRightChunks();
    }
    else if(difference.x < -dataPerChunkSide)
    {
      loadLeftChunks();
    } 
    
    if (difference.y > dataPerChunkSide)
    {
      loadBottomChunks();
    }
    else if (difference.y < -dataPerChunkSide)
    {
      loadTopChunks();
    }
  }

  void ProceduralDataGenerator::reload(const glm::vec2& position)
  {
    dataOrigin.x = std::floorf(position.x);
    dataOrigin.y = std::floorf(position.y);
    
    chunksOrigin.x = 0;
    chunksOrigin.y = 0;

    for (uint8_t x = 0; x < chunksPerSide; x++)
    {
      for (uint8_t y = 0; y < chunksPerSide; y++)
      {
        loadChunkData(x, y);
      }
    }

    stateSincePreviousFrame = ReloadedFlag;
    LOTUS_LOG_INFO("[Procedural Data Generator Log] All chunks reloaded");
  }

  void ProceduralDataGenerator::loadTopChunks()
  {
    dataOrigin.y -= dataPerChunkSide;
    chunksOrigin.y = (chunksOrigin.y + chunksPerSide - 1) % chunksPerSide;
    
    for (int x = 0; x < chunksPerSide; x++)
    {
      loadChunkData(x, getChunksTop());
    }

    stateSincePreviousFrame |= LoadedTopFlag;
    LOTUS_LOG_INFO("[Procedural Data Generator Log] Loaded top chunks");
  }

  void ProceduralDataGenerator::loadRightChunks()
  {
    dataOrigin.x += dataPerChunkSide;
    chunksOrigin.x = (chunksOrigin.x + 1) % chunksPerSide;

    for (int y = 0; y < chunksPerSide; y++)
    {
      loadChunkData(getChunksRight(), y);
    }

    stateSincePreviousFrame |= LoadedRightFlag;
    LOTUS_LOG_INFO("[Procedural Data Generator Log] Loaded right chunks");
  }

  void ProceduralDataGenerator::loadBottomChunks()
  {
    dataOrigin.y += dataPerChunkSide;
    chunksOrigin.y = (chunksOrigin.y + 1) % chunksPerSide;

    for (int x = 0; x < chunksPerSide; x++)
    {
      loadChunkData(x, getChunksBottom());
    }

    stateSincePreviousFrame |= LoadedBottomFlag;
    LOTUS_LOG_INFO("[Procedural Data Generator Log] Loaded bottom chunks");
  }

  void ProceduralDataGenerator::loadLeftChunks()
  {
    dataOrigin.x -= dataPerChunkSide;
    chunksOrigin.x = (chunksOrigin.x + chunksPerSide - 1) % chunksPerSide;
    
    for (int y = 0; y < chunksPerSide; y++)
    {
      loadChunkData(getChunksLeft(), y);
    }

    stateSincePreviousFrame |= LoadedLeftFlag;
    LOTUS_LOG_INFO("[Procedural Data Generator Log] Loaded left chunks");
  }

  void ProceduralDataGenerator::loadChunkData(const glm::uvec2& chunk)
  {
    loadChunkData(chunk.x, chunk.y);
  }

  void ProceduralDataGenerator::loadChunkData(uint8_t x, uint8_t y)
  {
    glm::ivec2 dataChunk((x - getChunksLeft() + chunksPerSide) % chunksPerSide, (y - getChunksTop() + chunksPerSide) % chunksPerSide);

    glm::ivec2 offset = dataOrigin - glm::ivec2((chunksPerSide * dataPerChunkSide) / 2) + dataChunk * static_cast<int>(dataPerChunkSide);
    
    float* chunkData = chunksData[y * chunksPerSide + x];

    noiseConfig.offset = offset;

    Perlin2DArray::fill(chunkData, dataPerChunkSide, dataPerChunkSide, noiseConfig);
  }

}