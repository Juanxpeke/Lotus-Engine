#pragma once

#include <glad/glad.h>
#include "../../math/types.h"

#include "../shader.h"

namespace Lotus
{
  namespace Traditional
  {
    enum class MaterialType
    {
      UnlitFlat,
      UnlitTextured,
      DiffuseFlat,
      DiffuseTextured,
      PBRFlat,
      PBRTextured,
      MaterialTypeCount
    };

    class Material
    {
    public:
      Material(const ShaderProgram& shaderProgram) : shaderID(shaderProgram.getProgramID()) {}
      virtual ~Material() = default;

      void setUniforms(
        const glm::mat4& perspectiveMatrix,
        const glm::mat4& viewMatrix,
        const glm::mat4& modelMatrix,
        const glm::vec3& cameraPosition);

      virtual void setMaterialUniforms(const glm::vec3& cameraPosition) = 0;

    protected:
      uint32_t shaderID;

    };
  }
}