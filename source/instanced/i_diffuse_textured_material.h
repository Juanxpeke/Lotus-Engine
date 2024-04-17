#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "../traditional/texture.h"
#include "i_material.h"

class DiffuseTexturedMaterial : public Material
{
public:

  DiffuseTexturedMaterial(const ShaderProgram& shaderProgram) : Material(shaderProgram), diffuseTexture(nullptr), materialTint(glm::vec3(1.0f))
  {
    // Since texture locations doesn't change, they have to be configured at the construction step
    glUseProgram(shaderID);
    glUniform1i(ShaderProgram::DiffuseTextureSamplerShaderLocation, ShaderProgram::DiffuseTextureUnit);
  }

  virtual void setMaterialUniforms(const glm::vec3& cameraPosition) override
  {
    // TODO: ASSERT(m_diffuseTexture != nullptr, "Material Error: Texture must be not nullptr for rendering to be possible");
    glBindTextureUnit(ShaderProgram::DiffuseTextureUnit, diffuseTexture->getID());
    glUniform3fv(ShaderProgram::MaterialTintShaderLocation, 1, glm::value_ptr(materialTint));
  }

  std::shared_ptr<Texture> getDiffuseTexture() const { return diffuseTexture; }

  void setDiffuseTexture(std::shared_ptr<Texture> texture) { diffuseTexture = texture; }

  const glm::vec3& getMaterialTint() const { return materialTint; }

  void setMaterialTint(const glm::vec3& tint) { materialTint = tint; }

private:
  std::shared_ptr<Texture> diffuseTexture;
  glm::vec3 materialTint;
};