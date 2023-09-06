#pragma once

#include <cstdint>
#include <string>

class Mesh
{
// friend class MeshManager;

public:
  enum class PrimitiveType {
    Plane,
    Cube,
    Sphere,
    PrimitiveCount
  };

  Mesh(PrimitiveType type);
  ~Mesh();
  uint32_t GetVertexArrayID() const noexcept { return m_vertexArrayID; }
  uint32_t GetIndexBufferCount() const noexcept { return m_indexBufferCount; }

private:
  void CreateSphere() noexcept;
  void CreateCube() noexcept;
  void CreatePlane() noexcept;

  uint32_t m_vertexArrayID;
  uint32_t m_vertexBufferID;
  uint32_t m_indexBufferID;
  uint32_t m_indexBufferCount;
};