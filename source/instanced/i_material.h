#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "i_shader_program.h"

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
friend class GraphicsBatch;

public:
  Material(const ShaderProgram& shaderProgram) : shaderID(shaderProgram.getProgramID()) {}
  virtual ~Material() = default;

  uint32_t getShaderID() { return shaderID; }

protected:
  struct MaterialData
  {
    glm::vec3 vec3A;  // 12
    int idA;          // 16
    glm::vec3 vec3B;  // 28
    int idB;          // 32
    int idC;          // 36
    int idD;          // 40
    int idE;          // 44
    int idF;          // 48
  };

  virtual void fillMaterialData(MaterialData& materialData) = 0;

  uint32_t shaderID;
};