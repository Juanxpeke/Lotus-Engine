#pragma once

#include <cstdint>
#include <string>
#include <vector>

class Mesh
{
public:
  enum class PrimitiveType
  {
    Plane,
    Cube,
    Sphere
  };

  Mesh(const std::string& filePath, bool flipUVs = false);
  Mesh(PrimitiveType type);
  ~Mesh();

  uint32_t getVertexArrayID() const noexcept { return vertexArrayID; }
  uint32_t getIndexBufferCount() const noexcept { return indexBufferCount; }

private:
  void createPlane() noexcept;
  void createSphere() noexcept;
  void createCube() noexcept;
  void createPrimitive(std::vector<float>& vertices, std::vector<unsigned int>& indices) noexcept;

  uint32_t vertexArrayID;
  uint32_t vertexBufferID;
  uint32_t indexBufferID;
  uint32_t indexBufferCount;
};