#pragma once

#include "../gpu_mesh.h"

namespace Lotus
{

  struct TraditionalRenderObject
  {
    uint32_t meshHandle;
    glm::mat4 model;
  };

  struct TraditionalRenderMesh
  {
    uint32_t references = 0;
    GPUMesh* gpuMesh = nullptr;
  };

}