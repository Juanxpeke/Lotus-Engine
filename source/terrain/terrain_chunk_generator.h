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
        uint16_t chunksPerSide,
        const Vec2i& dataOrigin = { 0, 0 });
    ~ProceduralDataGenerator();

    uint16_t getChunksPerSide() const { return chunksPerSide; };
    uint16_t getChunksAmount() const { return chunksPerSide * chunksPerSide; };

    const float* getChunkData(const Vec2i& chunk) const;
    const float* getChunkData(int x, int y) const;

    Vec2i getDataOrigin() const { return dataOrigin; }

    unsigned int getChunksTop() const { return chunksOrigin.y; };
    unsigned int getChunksRight() const { return (chunksOrigin.x + chunksPerSide - 1) % chunksPerSide; }
    unsigned int getChunksBottom() const { return (chunksOrigin.y + chunksPerSide - 1) % chunksPerSide; }
    unsigned int getChunksLeft() const { return chunksOrigin.x; };

    void updateTopChunks();
    void updateRightChunks();
    void updateBottomChunks();
    void updateLeftChunks();


  private:

    void generateChunkData(const Vec2i& chunk);
    void generateChunkData(int x, int y);

    uint16_t dataPerChunkSide;
    uint16_t chunksPerSide;

    Vec2i dataOrigin;
    Vec2u chunksOrigin;

    std::vector<float*> chunksData;

  };

}