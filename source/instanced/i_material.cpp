#include "i_material.h"

void Material::setUniforms(
    const glm::mat4& perspectiveMatrix,
    const glm::mat4& viewMatrix,
    const glm::mat4& modelMatrix,
    const glm::vec3& cameraPosition)
{
  glUseProgram(shaderID);
    
  const glm::mat4 mvpMatrix = perspectiveMatrix * viewMatrix * modelMatrix;
  const glm::mat4 modelInverseTransposeMatrix = glm::transpose(glm::inverse(modelMatrix));

  glUniformMatrix4fv(ShaderProgram::MvpMatrixShaderLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
  glUniformMatrix4fv(ShaderProgram::ModelMatrixShaderLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
  glUniformMatrix4fv(ShaderProgram::ModelInverseTransposeMatrixShaderLocation, 1, GL_FALSE, glm::value_ptr(modelInverseTransposeMatrix));
  
  setMaterialUniforms(cameraPosition);
}