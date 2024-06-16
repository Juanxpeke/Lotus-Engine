#pragma once

#include "../../math/gpu_primitives.h"
#include "../shader.h"

namespace Lotus
{
  enum class MaterialType
  {
    UnlitFlat,
    UnlitTextured,
    DiffuseFlat,
    DiffuseTextured,
    PBRFlat,
    PBRTextured,
    MaterialTypeCount
  };

  class Material
  {
  friend class Renderer;

  public:
    Material() : dirty(false) {}
    
    virtual ~Material() = default;

    MaterialType getType() { return type; };

    virtual GPUMaterialData getMaterialData() = 0;

  protected:
    MaterialType type;
    bool dirty;
  };
}

