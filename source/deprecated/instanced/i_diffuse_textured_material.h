#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "i_texture.h"
#include "i_material.h"

class DiffuseTexturedMaterial : public Material
{
public:

  DiffuseTexturedMaterial(const ShaderProgram& shaderProgram) : Material(shaderProgram), diffuseTexture(nullptr), diffuseTextureTint(glm::vec3(1.0f)) {}

  std::shared_ptr<Texture> getDiffuseTexture() const { return diffuseTexture; }
  const glm::vec3& getDiffuseTextureTint() const { return diffuseTextureTint; }

  void setDiffuseTexture(std::shared_ptr<Texture> texture)
  {
    if (texture == diffuseTexture) { return; }

    texture->increaseReferenceCount();

    if (diffuseTexture != nullptr)
    {
      diffuseTexture->decreaseReferenceCount();
    }
  
    diffuseTexture = texture;
    dirty = true;
  }

  void setDiffuseTextureTint(const glm::vec3& tint)
  {
    diffuseTextureTint = tint;
    dirty = true;
  }

protected:
  virtual void fillMaterialData(MaterialData& materialData) override
  {
    // TODO: ASSERT(m_diffuseTexture != nullptr, "Material Error: Texture must be not nullptr for rendering to be possible");
    materialData.uint64_0 = diffuseTexture->getHandle();
    materialData.vec3_0 = diffuseTextureTint;
  }

private:
  std::shared_ptr<Texture> diffuseTexture;
  glm::vec3 diffuseTextureTint;
};