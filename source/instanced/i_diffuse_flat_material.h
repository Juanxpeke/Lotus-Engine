#pragma once

#include <glm/glm.hpp>

#include "i_material.h"

class DiffuseFlatMaterial : public Material
{
public:

  DiffuseFlatMaterial(const ShaderProgram& shaderProgram) : Material(shaderProgram), diffuseColor(glm::vec3(1.0f)) {}

  const glm::vec3& getDiffuseColor() const { return diffuseColor; }

  void setDiffuseColor(const glm::vec3& color) { diffuseColor = color; }

protected:
  virtual void fillMaterialData(MaterialData& materialData) override
  {
    materialData.vec3A = diffuseColor;
  }
  
private:
  glm::vec3 diffuseColor;
};