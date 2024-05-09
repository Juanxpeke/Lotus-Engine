#pragma once

#include <cstdint>
#include <memory>
#include "../mesh.h"
#include "../mesh_gpu_link.h"

class TraditionalMeshGPULink : public MeshGPULink
{
public:
  TraditionalMeshGPULink() = delete;
  TraditionalMeshGPULink(std::shared_ptr<Mesh> mesh);

  ~TraditionalMeshGPULink();

  uint32_t getVertexArrayID() { return vertexArrayID; }
  uint32_t getVertexBufferID() { return vertexBufferID; }
  uint32_t getIndexBufferID() { return indexBufferID; }

private:
  uint32_t vertexArrayID;
  uint32_t vertexBufferID;
  uint32_t indexBufferID;
};