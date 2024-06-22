#pragma once

#include <cstdint>
#include <array>
#include "../math/noise.h"

namespace Lotus
{

  class TerrainChunkGenerator
  {
  public:
    static constexpr unsigned int ChunksPerSide = 4; 
    static constexpr unsigned int ChunksAmount = ChunksPerSide * ChunksPerSide;

    TerrainChunkGenerator(uint16_t terrainChunkSize);

    const float* getChunkData(int x, int y) const;

    int64_t getDataOriginX() const { return dataOriginX; }
    int64_t getDataOriginY() const { return dataOriginY; }

    unsigned int getLeft() const { return left; };
    unsigned int getUp() const { return up; };
    unsigned int getRight() const { return (left + ChunksPerSide - 1) % ChunksPerSide; }
    unsigned int getDown() const { return (up + ChunksPerSide - 1) % ChunksPerSide; }

    void updateUp();
    void updateRight();
    void updateDown();
    void updateLeft();

  private:

    void generateChunk(int x, int y, int worldX, int worldY);

    uint16_t chunkSize;
    std::array<Perlin2DArray*, ChunksAmount> chunks2DArrays;

    int64_t dataOriginX;
    int64_t dataOriginY;

    unsigned int left;
    unsigned int up;
  };

}