#pragma once

#include <cstdint>
#include <vector>
#include "../math/primitives.h"

namespace Lotus
{
  class GPUMesh
  {
  public:
    GPUMesh(const std::vector<Lotus::Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~GPUMesh();

    uint32_t getVertexArrayID() { return vertexArrayID; }
    uint32_t getIndicesCount() { return indicesCount; }

  private:
    uint32_t vertexArrayID;
    uint32_t vertexBufferID;
    uint32_t indexBufferID;
    uint32_t indicesCount;
  };
}
