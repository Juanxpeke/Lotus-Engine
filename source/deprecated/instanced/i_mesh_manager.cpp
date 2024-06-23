#include "i_mesh_manager.h"

std::string primitiveEnumToString(Mesh::PrimitiveType type)
{
  switch (type)
  {
  case Mesh::PrimitiveType::Plane:
    return "Plane";
    break;
  case Mesh::PrimitiveType::Cube:
    return "Cube";
    break;
  case Mesh::PrimitiveType::Sphere:
    return "Sphere";
    break;
  default:
    return "Sphere";
    break;
  }
}

std::shared_ptr<Mesh> MeshManager::loadMesh(Mesh::PrimitiveType type) noexcept
{
  const std::string primName = primitiveEnumToString(type);
  auto it = meshMap.find(primName);
  
  if (it != meshMap.end())
  {
    return it->second;
  }
  
  Mesh* meshPtr = new Mesh(type);
  std::shared_ptr<Mesh> sharedPtr = std::shared_ptr<Mesh>(meshPtr);

  // Before returning the loaded mesh, we add it to the map so future loads are faster
  meshMap.insert({ primName, sharedPtr });

  return sharedPtr;
}

std::shared_ptr<Mesh> MeshManager::loadMesh(const std::filesystem::path& filePath, bool flipUVs) noexcept
{
  const std::string& stringPath = filePath.string();
  
  // In case there already existed a loaded mesh with the given path referenced by the meshes map
  // it is returned immediately
  auto it = meshMap.find(stringPath);
  if (it != meshMap.end())
    return it->second;
  
  Mesh* meshPtr = new Mesh(stringPath, flipUVs);
  std::shared_ptr<Mesh> sharedPtr = std::shared_ptr<Mesh>(meshPtr);
  
  // Before returning the loaded mesh, we add it to the map so future loads are faster
  meshMap.insert({ stringPath, sharedPtr });
  return sharedPtr;
}

void MeshManager::cleanUnusedMeshes() noexcept
{
  /*
  * Remove all the map's pointers whose reference count is equal to one,
  * i.e., only the map's pointer is referencing that memory space
  */
  for (auto i = meshMap.begin(), last = meshMap.end(); i != last;)
  {
    if (i->second.use_count() == 1) {
      i = meshMap.erase(i);
    }
    else {
      ++i;
    }
  }
}

void MeshManager::shutDown() noexcept
{
  for (auto& entry : meshMap) {
    entry.second->clearData();
  }
  
  meshMap.clear();
}