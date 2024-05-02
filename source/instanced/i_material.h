#pragma once

#include <cstdint>
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
    glm::vec3 vec3_0;   // 12
    int int_0;          // 16
    glm::vec3 vec3_1;   // 28
    int int_1;          // 32
    uint64_t uint64_0;  // 40
    uint64_t uint64_1;  // 48
    uint64_t uint64_2;  // 56
    uint64_t uint64_3;  // 64
  };

  virtual void fillMaterialData(MaterialData& materialData) = 0;

  uint32_t shaderID;
};