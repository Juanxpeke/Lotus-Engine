#include "i_mesh_instance.h"

void MeshInstance::setMesh(std::shared_ptr<Mesh> mesh) noexcept
{
  if (mesh == nullptr || mesh == meshPtr)
  {
    return; // TODO: Log warning
  }

  if (!dirtyMeshPtr)
  {
    oldMeshPtr = meshPtr;
  }
  else if (mesh == oldMeshPtr)
  {
    meshPtr = mesh;
    dirtyMeshPtr = false;
    return;
  }
  
  meshPtr = mesh;
  dirtyMeshPtr = true;
}

void MeshInstance::setMaterial(std::shared_ptr<Material> material) noexcept
{
  if (material == nullptr || material == materialPtr)
  {
    return; // TODO: Log warning
  }

  if (!dirtyMaterialPtr)
  {
    oldMaterialPtr = materialPtr;
  }
  else if (material == oldMaterialPtr)
  {
    materialPtr = material;
    dirtyMaterialPtr = false;
    return;
  }
  
  materialPtr = material;
  dirtyMaterialPtr = true;
}