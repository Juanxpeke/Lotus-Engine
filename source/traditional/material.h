#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_program.h"

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