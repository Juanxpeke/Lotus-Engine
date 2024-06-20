#pragma once

#include <cstdint>
#include <array>
#include "../math/noise.h"

namespace Lotus
{

  class TerrainChunkGenerator
  {
  public:
    static constexpr unsigned int chunksPerSide = 4; 

    TerrainChunkGenerator(uint8_t terrainChunkSize);

    void updateTop();
    void updateRight();
    void updateDown();
    void updateLeft();

  private:

    void generateChunk(int x, int y);

    uint8_t chunkSize;
    std::array<Perlin2DArray*, chunksPerSide * chunksPerSide> chunks2DArrays;

    int64_t originX;
    int64_t originY;

    unsigned int topLeftChunkX;
    unsigned int topLeftChunkY;
  };

}