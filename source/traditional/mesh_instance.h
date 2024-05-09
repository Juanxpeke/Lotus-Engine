#pragma once

#include <iostream>
#include "../scene/node_3d.h"
#include "mesh.h"
#include "material.h"

class MeshInstance : public Node3D
{
public:

  MeshInstance(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) : meshPtr(mesh), materialPtr(material)
  {
    // ASSERT(mesh != nullptr, "StaticMeshComponent Error: Mesh pointer cannot be null.");
    // ASSERT(material != nullptr, "StaticMeshComponent Error: Material cannot be null.");
  }

  const std::shared_ptr<Mesh>& getMesh() const noexcept { return meshPtr; }
  const std::shared_ptr<Material>& getMaterial() const noexcept { return materialPtr; }

  void setMesh(std::shared_ptr<Mesh> mesh) noexcept;
  void setMaterial(std::shared_ptr<Material> material) noexcept;

private:
  std::shared_ptr<Mesh> meshPtr;
  std::shared_ptr<Material> materialPtr;
};