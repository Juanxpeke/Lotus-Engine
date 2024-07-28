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
#include "../shader.h"
#include "../material.h"
#include "../mesh_object.h"
#include "traditional_render_structures.h"

namespace Lotus
{

  class TraditionalObjectRenderer
  {
  public:
    TraditionalObjectRenderer();

    std::shared_ptr<MeshObject> createObject(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material);
  
    void render();
    void updateObjects();

  private:

    Handler<TraditionalRenderMesh> getMeshHandle(const std::shared_ptr<Mesh>& mesh);

    std::array<ShaderProgram, static_cast<unsigned int>(MaterialType::MaterialTypeCount ) * 2> shaders;

    std::vector<std::shared_ptr<MeshObject>> objects;
    std::vector<TraditionalRenderObject> renderObjects;

    std::vector<TraditionalRenderMesh> renderMeshes;

    std::unordered_map<std::shared_ptr<Mesh>, Handler<TraditionalRenderMesh>> meshMap;
  };

}