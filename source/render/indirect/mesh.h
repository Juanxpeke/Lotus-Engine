#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "../../math/primitives.h"

namespace Lotus
{
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
    
    const std::vector<Vertex>& getVertices() const { return vertices; }
    const std::vector<unsigned int>& getIndices() const { return indices; }

    uint32_t getIndicesCount() { return indices.size(); }

  private:
    Mesh(const std::string& filePath, bool flipUVs = false);
    Mesh(PrimitiveType type);
    
    void clearData() noexcept;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
  };
}