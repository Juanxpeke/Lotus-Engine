#pragma once

#include <memory>
#include <filesystem>
#include <unordered_map>

#include "i_mesh.h"

class MeshManager
{
public:
  using MeshMap = std::unordered_map<std::string, std::shared_ptr<Mesh>>;

  MeshManager(MeshManager const&) = delete;
  
  MeshManager& operator=(MeshManager const&) = delete;
  
  std::shared_ptr<Mesh> loadMesh(Mesh::PrimitiveType type) noexcept;
  std::shared_ptr<Mesh> loadMesh(const std::filesystem::path& filePath, bool flipUVs = false) noexcept;
  
  void cleanUnusedMeshes() noexcept;
  
  static MeshManager& getInstance() noexcept
  {
    static MeshManager instance;
    return instance;
  }

private:
  MeshManager() = default;

  void shutDown() noexcept;

  MeshMap meshMap;
};