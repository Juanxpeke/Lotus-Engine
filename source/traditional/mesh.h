#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "../math/primitives.h"
#include "mesh_gpu_link.h"

class MeshGPULink;

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

  const std::vector<LotusMath::Vertex>& getVertices() const { return vertices; }
  const std::vector<unsigned int>& getIndices() const { return indices; }

  uint32_t getIndicesCount() { return indices.size(); }

  bool isGPULinked() const
  {
    return GPULinked;
  }

  void setGPULink(std::shared_ptr<MeshGPULink> meshGPULink)
  {
    GPULink = meshGPULink;
    GPULinked = true;
  }

  const std::shared_ptr<MeshGPULink>& getGPULink()
  {
    return GPULink;
  }

private:
  Mesh(const std::string& filePath, bool flipUVs = false);
  Mesh(PrimitiveType type);
  
  void clearData() noexcept;

  std::vector<LotusMath::Vertex> vertices;
  std::vector<unsigned int> indices;

  std::shared_ptr<MeshGPULink> GPULink;
  bool GPULinked = false;
};