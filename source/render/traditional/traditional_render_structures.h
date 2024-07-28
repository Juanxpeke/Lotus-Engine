#pragma once

#include "../../math/types.h"
#include "../gpu_mesh.h"

namespace Lotus
{

  struct TraditionalRenderMesh
  {
      uint32_t references = 0;
      GPUMesh* gpuMesh = nullptr;
  };

  struct TraditionalRenderObject
  {
    Handler<TraditionalRenderMesh> meshHandle;
    glm::mat4 model;
  };

}