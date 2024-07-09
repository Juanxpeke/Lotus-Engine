#pragma once

#include <memory>
#include <array>
#include <vector>
#include <unordered_map>
#include "../../math/types.h"
#include "../../scene/transform.h"
#include "../../scene/camera.h"
#include "../../lighting/directional_light.h"
#include "../../lighting/point_light.h"
#include "../gpu_mesh.h"
#include "../shader.h"
#include "../material.h"
#include "../mesh_object.h"

namespace Lotus
{

  class TraditionalObjectRenderer
  {
  public:
    using GPUMeshMap = std::unordered_map<std::shared_ptr<Mesh>, std::shared_ptr<GPUMesh>>;

    TraditionalObjectRenderer();

    void render() noexcept;

    std::shared_ptr<MeshObject> createObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
  

  private:

    std::array<ShaderProgram, static_cast<unsigned int>(MaterialType::MaterialTypeCount ) * 2> shaders;

    std::vector<std::shared_ptr<MeshObject>> meshObjects;

    GPUMeshMap meshMap;
  };

}