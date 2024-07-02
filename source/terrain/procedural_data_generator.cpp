#include "procedural_data_generator.h"

#include <cmath>
#include "../util/log.h"

namespace Lotus
{
  constexpr char UnchangedFlag    = 0;
  constexpr char LoadedTopFlag    = 0b0001;
  constexpr char LoadedRightFlag  = 0b0010;
  constexpr char LoadedBottomFlag = 0b0100;
  constexpr char LoadedLeftFlag   = 0b1000;
  constexpr char ReloadedFlag     = LoadedTopFlag | LoadedRightFlag | LoadedBottomFlag | LoadedLeftFlag;

  ProceduralDataGenerator::ProceduralDataGenerator(
      uint16_t generatorDataPerChunkSide,
      uint8_t generatorChunksPerSide,
      const PerlinNoiseConfig& generatorNoiseConfig,
      const Vec2f& initialObserverPosition) : 
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

  const float* ProceduralDataGenerator::getChunkData(const Vec2u& chunk) const
  {
    return getChunkData(chunk.x, chunk.y);
  }

  const float* ProceduralDataGenerator::getChunkData(uint8_t x, uint8_t y) const
  {
    return chunksData[y * chunksPerSide + x];
  }

  bool ProceduralDataGenerator::updatedSincePreviousFrame(ProceduralDataDirection direction) const
  {
    switch (direction)
    {
      case ProceduralDataDirection::Top:
        return static_cast<bool>(stateSincePreviousFrame & LoadedTopFlag);
      case ProceduralDataDirection::Right:
        return static_cast<bool>(stateSincePreviousFrame & LoadedRightFlag);
      case ProceduralDataDirection::Bottom:
        return static_cast<bool>(stateSincePreviousFrame & LoadedBottomFlag);
      case ProceduralDataDirection::Left:
        return static_cast<bool>(stateSincePreviousFrame & LoadedLeftFlag);
      default:
        return false;
    }
  }

  void ProceduralDataGenerator::registerObserverPosition(const Vec2f& observerPosition)
  {
    Vec2f difference;

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

  void ProceduralDataGenerator::reload(const Vec2f& position)
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

  void ProceduralDataGenerator::loadChunkData(const Vec2u& chunk)
  {
    loadChunkData(chunk.x, chunk.y);
  }

  void ProceduralDataGenerator::loadChunkData(uint8_t x, uint8_t y)
  {
    Vec2i dataChunk((x - getChunksLeft() + chunksPerSide) % chunksPerSide, (y - getChunksTop() + chunksPerSide) % chunksPerSide);

    Vec2i offset = dataOrigin - Vec2i((chunksPerSide * dataPerChunkSide) / 2) + dataChunk * dataPerChunkSide;
    
    float* chunkData = chunksData[y * chunksPerSide + x];

    noiseConfig.offset = offset;

    Perlin2DArray::fill(chunkData, dataPerChunkSide, dataPerChunkSide, noiseConfig);
  }

}