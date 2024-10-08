#pragma once

#include <iostream>
#include "../util/log.h"
#include "../scene/node_3d.h"
#include "mesh.h"
#include "gpu_mesh.h"
#include "material.h"

namespace Lotus
{
  class MeshObject : public Node3D
  {
  friend class TraditionalObjectRenderer;
  friend class IndirectObjectRenderer;

  public:

    MeshObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) :
      meshPtr(mesh),
      materialPtr(material),
      meshDirty(false),
      materialDirty(false),
      shaderDirty(false)
    {
      LOTUS_ASSERT(mesh != nullptr, "[Mesh Object Error] Mesh pointer cannot be null");
      LOTUS_ASSERT(material != nullptr, "[Mesh Object Error] Material pointer cannot be null");
    }

    const std::shared_ptr<Mesh>& getMesh() const noexcept { return meshPtr; }
    const std::shared_ptr<Material>& getMaterial() const noexcept { return materialPtr; }

    void setMesh(std::shared_ptr<Mesh> mesh) noexcept
    {
      LOTUS_ASSERT(mesh != nullptr, "[Mesh Object Error] Mesh pointer cannot be null");
      
      if (mesh == meshPtr)
      {
        LOTUS_LOG_WARN("[Mesh Object Warning] Tried to set mesh that is already being used");
        return;
      }

      meshPtr = mesh;
      meshDirty = true;
    }
    
    void setMaterial(std::shared_ptr<Material> material) noexcept
    {
      LOTUS_ASSERT(material != nullptr, "[Mesh Object Error] Material pointer cannot be null");
      
      if (material == materialPtr)
      {
        LOTUS_LOG_WARN("[Mesh Object Warning] Tried to set same material as the object's");
        return;
      }

      if (material->getType() != materialPtr->getType())
      {
        shaderDirty = true;
      }

      materialPtr = material;
      materialDirty = true;
    }

  private:
    std::shared_ptr<Mesh> meshPtr;
    std::shared_ptr<Material> materialPtr;

    bool meshDirty;
    bool materialDirty;
    bool shaderDirty;
  };
}