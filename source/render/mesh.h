#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "../math/types.h"

namespace Lotus
{
  struct MeshVertex
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent;
    glm::vec3 bitangent;
  };

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

    Mesh() = default;
    Mesh(const Mesh& other) : vertices(other.vertices), indices(other.indices) {}
    ~Mesh();
    
    const std::vector<MeshVertex>& getVertices() const { return vertices; }
    const std::vector<unsigned int>& getIndices() const { return indices; }

    uint32_t getIndicesCount() { return indices.size(); }

  protected:
    Mesh(const std::string& filePath, bool flipUVs = false);
    Mesh(PrimitiveType type);

    std::vector<MeshVertex> vertices;
    std::vector<unsigned int> indices;
  };

  class Plane : public Mesh
  {
  public:
    Plane();
  };

  class Cube : public Mesh
  {
  public:
    Cube();
  };

  class Sphere : public Mesh
  {
  public:
    Sphere();
  };
}