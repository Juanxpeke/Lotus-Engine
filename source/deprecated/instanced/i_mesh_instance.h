#pragma once

#include <iostream>
#include "../scene/node_3d.h"
#include "i_mesh.h"
#include "i_material.h"

class MeshInstance : public Node3D
{
public:

  MeshInstance(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) :
    meshPtr(mesh), materialPtr(material), oldMeshPtr(mesh), oldMaterialPtr(material)
  {
    // ASSERT(mesh != nullptr, "StaticMeshComponent Error: Mesh pointer cannot be null.");
    // ASSERT(material != nullptr, "StaticMeshComponent Error: Material cannot be null.");
  }

  uint32_t getMeshIndexCount() const noexcept { return meshPtr->getIndexBufferCount(); }
  uint32_t getMeshVAO() const noexcept { return meshPtr->getVertexArrayID(); }

  std::shared_ptr<Mesh> getMesh() const noexcept { return meshPtr; }
  std::shared_ptr<Material> getMaterial() const noexcept { return materialPtr; }
  std::shared_ptr<Mesh> getOldMesh() const noexcept { return oldMeshPtr; }
  std::shared_ptr<Material> getOldMaterial() const noexcept { return oldMaterialPtr; }

  bool hasDirtyMeshPtr() const noexcept { return dirtyMeshPtr; }
  bool hasDirtyMaterialPtr() const noexcept { return dirtyMaterialPtr; }

  void cleanDirtyMeshPtr() noexcept { dirtyMeshPtr = false; }
  void cleanDirtyMaterialPtr() noexcept { dirtyMaterialPtr = false; }

  void setMesh(std::shared_ptr<Mesh> mesh) noexcept;
  void setMaterial(std::shared_ptr<Material> material) noexcept;
  

private:
  std::shared_ptr<Mesh> meshPtr;
  std::shared_ptr<Material> materialPtr;

  std::shared_ptr<Mesh> oldMeshPtr;
  std::shared_ptr<Material> oldMaterialPtr;

  bool dirtyMeshPtr = false;
  bool dirtyMaterialPtr = false;
};