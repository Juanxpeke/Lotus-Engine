#pragma once

#include <memory>
#include "../../math/types.h"

#include "texture.h"
#include "material.h"

namespace Lotus
{
  namespace Traditional
  {
    class DiffuseTexturedMaterial : public Material
    {
    public:

      DiffuseTexturedMaterial(const ShaderProgram& shaderProgram) : Material(shaderProgram), diffuseTexture(nullptr), diffuseTextureTint(glm::vec3(1.0f))
      {
        // Since texture locations doesn't change, they have to be configured at the construction step
        glUseProgram(shaderID);
        glUniform1i(ShaderProgram::DiffuseTextureSamplerShaderLocation, ShaderProgram::DiffuseTextureUnit);
      }

      virtual void setMaterialUniforms(const glm::vec3& cameraPosition) override
      {
        // TODO: ASSERT(m_diffuseTexture != nullptr, "Material Error: Texture must be not nullptr for rendering to be possible");
        glBindTextureUnit(ShaderProgram::DiffuseTextureUnit, diffuseTexture->getID());
        glUniform3fv(ShaderProgram::MaterialTintShaderLocation, 1, glm::value_ptr(diffuseTextureTint));
      }

      std::shared_ptr<Texture> getDiffuseTexture() const { return diffuseTexture; }
      const glm::vec3& getDiffuseTextureTint() const { return diffuseTextureTint; }
      
      void setDiffuseTexture(std::shared_ptr<Texture> texture) { diffuseTexture = texture; }
      void setDiffuseTextureTint(const glm::vec3& tint) { diffuseTextureTint = tint; }

    private:
      std::shared_ptr<Texture> diffuseTexture;
      glm::vec3 diffuseTextureTint;
    };
  }
}
