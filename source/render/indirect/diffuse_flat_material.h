#pragma once

#include <glm/glm.hpp>
#include "material.h"

namespace Lotus
{
  class DiffuseFlatMaterial : public Material
  {
  public:
    DiffuseFlatMaterial() : diffuseColor(glm::vec3(1.0f))
    {
      this->type = MaterialType::DiffuseFlat;
    }

    glm::vec3 getDiffuseColor() const { return diffuseColor; }

    void setDiffuseColor(const glm::vec3& color)
    {
      if (color == diffuseColor) { return; }

      diffuseColor = color;
      dirty = true;
    }

    virtual GPUMaterialData getMaterialData() override
    {
      GPUMaterialData materialData;
      materialData.vec3_0 = diffuseColor;
      return materialData;
    }

  private:
    glm::vec3 diffuseColor;
  };
}