#pragma once

#include "../scene/transform.h"
#include "mesh.h"
#include "material.h"

class MeshInstance
{
public:

  MeshInstance(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) : meshPtr(mesh), materialPtr(material)
  {
    // ASSERT(mesh != nullptr, "StaticMeshComponent Error: Mesh pointer cannot be null.");
    // ASSERT(material != nullptr, "StaticMeshComponent Error: Material cannot be null.");
  }

  Transform& getTransform()
  {
    return transform;
  }

  uint32_t getMeshIndexCount() const noexcept
  {
    return meshPtr->getIndexBufferCount();
  }

  uint32_t getMeshVAO() const noexcept
  {
    return meshPtr->getVertexArrayID();
  }

  std::shared_ptr<Material> getMaterial() const noexcept
  {
    return materialPtr;
  }

  void setMaterial(std::shared_ptr<Material> material) noexcept
  {
    if (material != nullptr)
    {
      materialPtr = material;
    }
  }
  

private:
  Transform transform;
  std::shared_ptr<Mesh> meshPtr;
  std::shared_ptr<Material> materialPtr;
};