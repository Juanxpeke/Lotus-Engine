#pragma once

#include "../util/log.h"
#include "../util/opengl_entry.h"
#include "../math/types.h"
#include "gpu_texture.h"
#include "material.h"

namespace Lotus
{

  class DiffuseTexturedMaterial : public Material
  {
  public:

    DiffuseTexturedMaterial() : diffuseTexture(nullptr), diffuseTextureTint(glm::vec3(1.0f))
    {
      this->type = MaterialType::DiffuseTextured;
    }
    
    std::shared_ptr<GPUTexture> getDiffuseTexture() const { return diffuseTexture; }
    const glm::vec3& getDiffuseTextureTint() const { return diffuseTextureTint; }
    
    void setDiffuseTexture(std::shared_ptr<GPUTexture> texture)
    {
      LOTUS_ASSERT(texture != nullptr, "[Material Error] Texture pointer cannot be null");

      if (texture == diffuseTexture)
      {
        LOTUS_LOG_WARN("[Material Warning] Tried to set texture that is already being used");
        return;
      }

      diffuseTexture = texture;
      dirty = true;
    }
    
    void setDiffuseTextureTint(const glm::vec3& tint)
    {
      if (tint == diffuseTextureTint)
      {
        LOTUS_LOG_WARN("[Material Warning] Tried to set tint that is already being used");
        return;
      }

      diffuseTextureTint = tint;
      dirty = true;
    }
    
    virtual void setMaterialUniforms() override
    {
      LOTUS_ASSERT(diffuseTexture != nullptr, "[Material Error] Texture pointer cannot be null");

      glUniform1i(ShaderProgram::DiffuseTextureSamplerShaderLocation, ShaderProgram::DiffuseTextureUnit);
      glBindTextureUnit(ShaderProgram::DiffuseTextureUnit, diffuseTexture->getID());
      glUniform3fv(ShaderProgram::MaterialTintShaderLocation, 1, glm::value_ptr(diffuseTextureTint));
    }

    virtual GPUMaterialData getMaterialData() override
    {
      LOTUS_ASSERT(diffuseTexture != nullptr, "[Material Error] Texture pointer cannot be null");

      GPUMaterialData materialData;
      materialData.vec3_0 = diffuseTextureTint;
      materialData.uint64_0 = diffuseTexture->getHandle();
      return materialData;
    }

  private:
    std::shared_ptr<GPUTexture> diffuseTexture;
    glm::vec3 diffuseTextureTint;
  };

}
