#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "../math/primitives.h"

class Mesh
{
friend class MeshManager;

public:
  enum class PrimitiveType
  {
    Plane,
    Cube,
    Sphere
  };

  ~Mesh();

  uint32_t getVertexArrayID() const noexcept { return vertexArrayID; }
  uint32_t getIndexBufferCount() const noexcept { return indexBufferCount; }

private:
  Mesh(const std::string& filePath, bool flipUVs = false);
  Mesh(PrimitiveType type);

  void createMesh(std::vector<LotusMath::Vertex>& vertices, std::vector<unsigned int>& indices) noexcept;
  
  void clearData() noexcept;

  uint32_t vertexArrayID;
  uint32_t vertexBufferID;
  uint32_t indexBufferID;
  uint32_t indexBufferCount;
};