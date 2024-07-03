#pragma once

#include <cstdint>
#include <vector>
#include "../math/linear_algebra.h"
#include "../math/noise.h"

namespace Lotus
{
  enum class ProceduralDataUpdate
  {
    Top,
    Right,
    Bottom,
    Left,
    Everything
  };

  class ProceduralDataGenerator
  {
  public:

    ProceduralDataGenerator(
        uint16_t dataPerChunkSide,
        uint8_t chunksPerSide,
        const PerlinNoiseConfig& noiseConfig,
        const Vec2f& initialObserverPosition = { 0, 0 });
    ~ProceduralDataGenerator();

    uint16_t getDataPerChunkSide() const { return dataPerChunkSide; }
    uint8_t getChunksPerSide() const { return chunksPerSide; };
    uint32_t getChunksAmount() const { return chunksPerSide * chunksPerSide; };

    const float* getChunkData(const Vec2u& chunk) const;
    const float* getChunkData(uint8_t x, uint8_t y) const;

    Vec2i getDataOrigin() const { return dataOrigin; }

    unsigned int getChunksTop() const { return chunksOrigin.y; };
    unsigned int getChunksRight() const { return (chunksOrigin.x + chunksPerSide - 1) % chunksPerSide; }
    unsigned int getChunksBottom() const { return (chunksOrigin.y + chunksPerSide - 1) % chunksPerSide; }
    unsigned int getChunksLeft() const { return chunksOrigin.x; };

    bool updatedSincePreviousFrame(ProceduralDataUpdate direction) const;

    void registerObserverPosition(const Vec2f& observerPosition);

  private:
    
    void reload(const Vec2f& position);
    void loadTopChunks();
    void loadRightChunks();
    void loadBottomChunks();
    void loadLeftChunks();

    void loadChunkData(const Vec2u& chunk);
    void loadChunkData(uint8_t x, uint8_t y);

    uint16_t dataPerChunkSide;
    uint8_t chunksPerSide;

    Vec2i dataOrigin;
    Vec2u chunksOrigin;

    PerlinNoiseConfig noiseConfig;

    char stateSincePreviousFrame;

    std::vector<float*> chunksData;
  };

}