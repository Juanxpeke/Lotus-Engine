#pragma once

#include <glm/glm.hpp>

#include "material.h"

class DiffuseFlatMaterial : public Material
{
public:

  DiffuseFlatMaterial(const ShaderProgram& shaderProgram) : Material(shaderProgram), m_diffuseColor(glm::vec3(1.0f)) {}

  virtual void setMaterialUniforms(const glm::vec3& cameraPosition) override
  {
    glUniform3fv(ShaderProgram::DiffuseColorShaderLocation, 1, glm::value_ptr(m_diffuseColor));
  }

  const glm::vec3& getDiffuseColor() const { return m_diffuseColor; }

  void setDiffuseColor(const glm::vec3& color) { m_diffuseColor = color; }
  
private:
  glm::vec3 m_diffuseColor;
};