#pragma once

#include "../../math/types.h"
#include "../gpu_mesh.h"

namespace Lotus
{

  struct TraditionalRenderMesh
  {
    GPUMesh* gpuMesh = nullptr;
    uint32_t references = 0;
  };

  struct TraditionalRenderObject
  {
    Handler<TraditionalRenderMesh> mesh;
    glm::mat4 model;
  };

}