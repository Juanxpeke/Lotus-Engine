#pragma once

#include <glm/glm.hpp>

#include "material.h"

class DiffuseFlatMaterial : public Material
{
public:

  DiffuseFlatMaterial(const ShaderProgram& shaderProgram) : Material(shaderProgram), diffuseColor(glm::vec3(1.0f)) {}

  virtual void setMaterialUniforms(const glm::vec3& cameraPosition) override
  {
    glUniform3fv(ShaderProgram::DiffuseColorShaderLocation, 1, glm::value_ptr(diffuseColor));
  }

  const glm::vec3& getDiffuseColor() const { return diffuseColor; }

  void setDiffuseColor(const glm::vec3& color) { diffuseColor = color; }
  
private:
  glm::vec3 diffuseColor;
};