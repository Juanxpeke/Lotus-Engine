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

    MeshInstance(std::shared_ptr<Mesh> mesh) : meshPtr(mesh), meshDirty(false), materialDirty(false), shaderDirty(false)
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
    std::shared_ptr<Mesh> meshPtr;
    // std::shared_ptr<Material> materialPtr;

    bool meshDirty;
    bool materialDirty;
    bool shaderDirty;
  };
}