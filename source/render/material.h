#pragma once

#include "../math/types.h"
#include "gpu_structures.h"
#include "shader.h"

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
  friend class IndirectObjectRenderer;

  public:
    Material() : dirty(false) {}
    
    virtual ~Material() = default;

    MaterialType getType() { return type; };

    void setUniforms(const glm::mat4& modelMatrix);

    virtual void setMaterialUniforms() = 0;

    virtual GPUMaterialData getMaterialData() = 0;

  protected:
    MaterialType type;
    bool dirty;
  };
}

