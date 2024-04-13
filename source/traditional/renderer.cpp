#include "renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "mesh.h"
#include "diffuse_flat_material.h"
#include "diffuse_textured_material.h"

void Renderer::startUp() noexcept
{
  // shaders[static_cast<unsigned int >(MaterialType::DiffuseFlat)] = ShaderProgram(SourcePath("source/Rendering/Shaders/DiffuseFlat.vs"), SourcePath("source/Rendering/Shaders/DiffuseFlat.ps"));
  // shaders[static_cast<unsigned int >(MaterialType::DiffuseTextured)] = ShaderProgram(SourcePath("source/Rendering/Shaders/DiffuseTextured.vs"), SourcePath("source/Rendering/Shaders/DiffuseTextured.ps"));

  glEnable(GL_DEPTH_TEST);

  glGenBuffers(1, &lightDataUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, lightDataUBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(Lights), NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightDataUBO);
}

void Renderer::shutDown() noexcept
{
  glDeleteBuffers(1, &lightDataUBO);
}

void Renderer::render() noexcept
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glm::mat4 viewMatrix;
  glm::mat4 projectionMatrix;
  glm::vec3 cameraPosition = glm::vec3(0.0f);

  // glBindBuffer(GL_UNIFORM_BUFFER, lightDataUBO);
  // glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Lights), nullptr);
  // glBindBuffer(GL_UNIFORM_BUFFER, 0);

  /*
  for (decltype(staticMeshDataManager.GetCount()) i = 0;
    i < staticMeshDataManager.GetCount();
    i++)
  {
    glBindVertexArray(staticMesh.GetMeshVAOID());
    staticMesh.m_materialPtr->SetUniforms(projectionMatrix, viewMatrix, transform->GetModelMatrix(), cameraPosition);
    glDrawElements(GL_TRIANGLES, staticMesh.GetMeshIndexCount(), GL_UNSIGNED_INT, 0); 
  }
  */
}

std::shared_ptr<Material> Renderer::createMaterial(MaterialType type)
{
  unsigned int offset = static_cast<unsigned int>(type);

  switch (type)
  {
  case MaterialType::DiffuseFlat:
    return std::make_shared<DiffuseFlatMaterial>(shaders[offset]);
    break;
  case MaterialType::DiffuseTextured:
    return std::make_shared<DiffuseTexturedMaterial>(shaders[offset]);
    break;
  case MaterialType::MaterialTypeCount:
    return std::make_shared<DiffuseFlatMaterial>(shaders[offset]);
    break;
  default:
    return nullptr;
    break;
  }
}