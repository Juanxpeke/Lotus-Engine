#pragma once

#include "../math/types.h"
#include "material.h"

namespace Lotus
{
  class UnlitFlatMaterial : public Material
  {
  public:
    UnlitFlatMaterial() : unlitColor(glm::vec3(1.0f))
    {
      type = MaterialType::UnlitFlat;
    }

    glm::vec3 getUnlitColor() const { return unlitColor; }

    void setUnlitColor(const glm::vec3& color)
    {
      if (color == unlitColor)
      {
        return;
      }

      unlitColor = color;
      dirty = true;
    }

    virtual void setMaterialUniforms() override
    {
      glUniform3fv(ShaderProgram::DiffuseColorShaderLocation, 1, glm::value_ptr(unlitColor));
    }

    virtual GPUMaterialData getMaterialData() override
    {
      GPUMaterialData materialData;
      materialData.vec3_0 = unlitColor;
      return materialData;
    }

  private:
    glm::vec3 unlitColor;
  };
}