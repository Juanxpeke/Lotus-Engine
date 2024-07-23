#include "material.h"

#include "../util/opengl_entry.h"

namespace Lotus
{

  void Material::setUniforms(const glm::mat4& modelMatrix)
  {
    const glm::mat4 modelInverseTransposeMatrix = glm::transpose(glm::inverse(modelMatrix));

    glUniformMatrix4fv(ShaderProgram::ModelMatrixShaderLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(ShaderProgram::ModelInverseTransposeMatrixShaderLocation, 1, GL_FALSE, glm::value_ptr(modelInverseTransposeMatrix));

    setMaterialUniforms();
  }

}