#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "../traditional/texture.h"
#include "i_material.h"

class DiffuseTexturedMaterial : public Material
{
public:

  DiffuseTexturedMaterial(const ShaderProgram& shaderProgram) : Material(shaderProgram), diffuseTexture(nullptr), materialTint(glm::vec3(1.0f)) {}

  std::shared_ptr<Texture> getDiffuseTexture() const { return diffuseTexture; }
  const glm::vec3& getMaterialTint() const { return materialTint; }

  void setDiffuseTexture(std::shared_ptr<Texture> texture) { diffuseTexture = texture; }
  void setMaterialTint(const glm::vec3& tint) { materialTint = tint; }

protected:
  virtual void fillMaterialData(MaterialData& materialData) override
  {
    // TODO: ASSERT(m_diffuseTexture != nullptr, "Material Error: Texture must be not nullptr for rendering to be possible");
    materialData.idA = diffuseTexture->getID();
    materialData.vec3A = materialTint;
  }

private:
  std::shared_ptr<Texture> diffuseTexture;
  glm::vec3 materialTint;
};