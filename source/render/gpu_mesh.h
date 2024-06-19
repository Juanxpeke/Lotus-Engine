#pragma once

#include <cstdint>
#include <vector>
#include "../math/primitives.h"
#include "gpu_buffer.h"

namespace Lotus
{
  class GPUMesh
  {
  public:
    GPUMesh(const std::vector<Lotus::Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~GPUMesh();

    GPUMesh& operator=(const GPUMesh& other) = delete;

    uint32_t getVertexArrayID() const { return vertexArrayID; }
    uint32_t getIndicesCount() const { return indicesCount; }

  private:
    uint32_t vertexArrayID;
    VertexBuffer vertexBuffer;
    IndexBuffer indexBuffer;

    uint32_t indicesCount;
  };
}
