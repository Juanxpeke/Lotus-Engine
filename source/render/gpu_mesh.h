#pragma once

#include <vector>
#include "mesh.h"
#include "../math/types.h"
#include "gpu_buffer.h"

namespace Lotus
{
  class GPUMesh
  {
  public:
    GPUMesh(const Mesh& mesh);
    GPUMesh(const std::vector<Lotus::MeshVertex>& vertices, const std::vector<unsigned int>& indices);
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
