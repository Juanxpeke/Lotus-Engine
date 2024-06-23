#pragma once

#include <cstdint>
#include <vector>
#include "../math/linear_algebra.h"
#include "../math/noise.h"

namespace Lotus
{

  class ProceduralDataGenerator
  {
  public:

    ProceduralDataGenerator(
        uint16_t dataPerChunkSide,
        uint16_t chunksPerSide = 8,
        Vec2i dataOrigin = { 0, 0 },
        uint32_t seed = 0);
    ~ProceduralDataGenerator();

    uint16_t getChunksPerSide() const { return chunksPerSide; };
    uint16_t getChunksAmount() const { return chunksPerSide * chunksPerSide; };

    const float* getChunkData(int x, int y) const;

    Vec2i getDataOrigin() const { return dataOrigin; }

    unsigned int getLeft() const { return chunksOrigin.x; };
    unsigned int getUp() const { return chunksOrigin.y; };
    unsigned int getRight() const { return (chunksOrigin.x + chunksPerSide - 1) % chunksPerSide; }
    unsigned int getDown() const { return (chunksOrigin.y + chunksPerSide - 1) % chunksPerSide; }

    void updateUp();
    void updateRight();
    void updateDown();
    void updateLeft();

  private:

    void generateChunkData(int x, int y, int worldX, int worldY);

    uint16_t dataPerChunkSide;
    uint16_t chunksPerSide;
    std::vector<float*> chunksData;

    Vec2i dataOrigin;
    Vec2u chunksOrigin;
  };

}