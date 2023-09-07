#pragma once

#include <cstdint>
#include <string>
#include <vector>

class Mesh
{
public:
  enum class PrimitiveType {
    Plane,
    Cube,
    Sphere
  };

  Mesh(PrimitiveType type);
  uint32_t getVertexArrayID() const noexcept { return m_vertexArrayID; }
  uint32_t getIndexBufferCount() const noexcept { return m_indexBufferCount; }
  ~Mesh();

private:
  void createPlane() noexcept;
  void createSphere() noexcept;
  void createCube() noexcept;
  void createPrimitive(std::vector<float>& vertices, std::vector<unsigned int>& indices) noexcept;

  uint32_t m_vertexArrayID;
  uint32_t m_vertexBufferID;
  uint32_t m_indexBufferID;
  uint32_t m_indexBufferCount;
};