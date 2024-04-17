#pragma once

#include <iostream>
#include "../scene/node_3d.h"
#include "i_mesh.h"
#include "i_material.h"

class MeshInstance : public Node3D
{
public:

  MeshInstance(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) : meshPtr(mesh), materialPtr(material)
  {
    // ASSERT(mesh != nullptr, "StaticMeshComponent Error: Mesh pointer cannot be null.");
    // ASSERT(material != nullptr, "StaticMeshComponent Error: Material cannot be null.");
    // updateGraphicsBatch();
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
  void updateGraphicsBatch();

  std::shared_ptr<Mesh> meshPtr;
  std::shared_ptr<Material> materialPtr;
};