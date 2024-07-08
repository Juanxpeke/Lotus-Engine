#pragma once

#include <memory>
#include <filesystem>
#include <unordered_map>

#include "mesh.h"

namespace Lotus
{
  class MeshManager
  {
  public:
    using MeshMap = std::unordered_map<std::string, std::shared_ptr<Mesh>>;

    MeshManager(MeshManager const&) = delete;
    
    MeshManager& operator=(MeshManager const&) = delete;
    
    static MeshManager& getInstance() noexcept
    {
      static MeshManager instance;
      return instance;
    }

    std::shared_ptr<Mesh> loadMesh(Mesh::PrimitiveType type) noexcept;
    std::shared_ptr<Mesh> loadMesh(const std::filesystem::path& filePath, bool flipUVs = false) noexcept;
    
    void cleanUnusedMeshes() noexcept;

  private:
    MeshManager() = default;

    MeshMap meshMap;
  };
}