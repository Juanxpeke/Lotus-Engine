#pragma once

#include "../../math/gpu_primitives.h"
#include "shader_program.h"

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
    Material(const ShaderProgram& shaderProgram) : shaderID(shaderProgram.getProgramID()), dirty(false) {}
    virtual ~Material() = default;

    uint32_t getShaderID() { return shaderID; };

    virtual GPUMaterialData getMaterialData() = 0;

  protected:
    uint32_t shaderID;
    bool dirty;
  };
}

