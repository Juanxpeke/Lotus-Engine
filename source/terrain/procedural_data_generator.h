#pragma once

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>
#include "../math/noise.h"

namespace Lotus
{
  enum class ProceduralUpdateRegion
  {
    TopChunks,
    RightChunks,
    BottomChunks,
    LeftChunks,
    Everything
  };

  class ProceduralDataGenerator
  {
  public:

    ProceduralDataGenerator(
        uint16_t dataPerChunkSide,
        uint8_t chunksPerSide,
        const PerlinNoiseConfig& noiseConfig,
        const glm::vec2& initialObserverPosition = { 0, 0 });
    ~ProceduralDataGenerator();

    uint16_t getDataPerChunkSide() const { return dataPerChunkSide;                 }
    uint8_t  getChunksPerSide()    const { return chunksPerSide;                    }
    uint32_t getDataAmount()       const { return dataPerChunkSide * chunksPerSide; }
    uint16_t getChunksAmount()     const { return chunksPerSide * chunksPerSide;    }

    const float* getChunkData(const glm::uvec2& chunk) const;
    const float* getChunkData(uint8_t x, uint8_t y) const;

    glm::ivec2 getDataOrigin()   const { return dataOrigin;   }
    glm::uvec2 getChunksOrigin() const { return chunksOrigin; }

    unsigned int getChunksTop()    const { return chunksOrigin.y;                                       }
    unsigned int getChunksRight()  const { return (chunksOrigin.x + chunksPerSide - 1) % chunksPerSide; }
    unsigned int getChunksBottom() const { return (chunksOrigin.y + chunksPerSide - 1) % chunksPerSide; }
    unsigned int getChunksLeft()   const { return chunksOrigin.x;                                       }

    bool updatedSincePreviousFrame(ProceduralUpdateRegion region) const;

    void registerObserverPosition(const glm::vec2& observerPosition);

  private:
    
    void reload(const glm::vec2& position);
    void loadTopChunks();
    void loadRightChunks();
    void loadBottomChunks();
    void loadLeftChunks();

    void loadChunkData(const glm::uvec2& chunk);
    void loadChunkData(uint8_t x, uint8_t y);

    uint16_t dataPerChunkSide;
    uint8_t chunksPerSide;

    glm::ivec2 dataOrigin;
    glm::uvec2 chunksOrigin;

    PerlinNoiseConfig noiseConfig;

    char stateSincePreviousFrame;

    std::vector<float*> chunksData;
  };

}