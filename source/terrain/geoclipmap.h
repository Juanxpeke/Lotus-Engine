#pragma once

#include <vector>
#include <memory>
#include "../math/types.h"
#include "../render/gpu_mesh.h"

namespace Lotus
{
  class GeoClipmap
  {
  public:
    enum MeshType
    {
      TILE,
      FILLER,
      TRIM,
      CROSS,
      SEAM
    };

    static std::vector<std::shared_ptr<GPUMesh>> generate(uint32_t tileResolution);
  };
}
