#pragma once

#include <iostream>
#include "../../scene/node_3d.h"
#include "mesh.h"
// #include "material.h"

namespace Lotus
{
  class MeshInstance : public Node3D
  {
  friend class Renderer;

  public:

    MeshInstance(std::shared_ptr<Mesh> mesh) : meshPtr(mesh)
    {
      // ASSERT(mesh != nullptr, "StaticMeshComponent Error: Mesh pointer cannot be null.");
      // ASSERT(material != nullptr, "StaticMeshComponent Error: Material cannot be null.");
    }

    const std::shared_ptr<Mesh>& getMesh() const noexcept { return meshPtr; }
    // const std::shared_ptr<Material>& getMaterial() const noexcept { return materialPtr; }

    void setMesh(std::shared_ptr<Mesh> mesh) noexcept
    {
      if (mesh == nullptr || mesh == meshPtr) { return; }

      meshPtr = mesh;
      meshDirty = true;
    }
    // void setMaterial(std::shared_ptr<Material> material) noexcept;



  private:

    bool isDirty() {  return transformDirty || meshDirty || materialDirty; }
    bool isMeshDirty() { return meshDirty; }
    bool isMaterialDirty() { return materialDirty; }

    std::shared_ptr<Mesh> meshPtr;
    // std::shared_ptr<Material> materialPtr;

    bool transformDirty;
    bool meshDirty;
    bool materialDirty;
  };
}