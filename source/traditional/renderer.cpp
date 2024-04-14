#include "renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../path_manager.h"
#include "mesh.h"
#include "diffuse_flat_material.h"
#include "diffuse_textured_material.h"

void Renderer::startUp() noexcept
{
  shaders[static_cast<unsigned int>(MaterialType::DiffuseFlat)] = ShaderProgram(shaderPath("diffuse_flat.vert"), shaderPath("diffuse_flat.frag"));
  shaders[static_cast<unsigned int>(MaterialType::DiffuseTextured)] = ShaderProgram(shaderPath("diffuse_textured.vert"), shaderPath("diffuse_textured.frag"));

  glEnable(GL_DEPTH_TEST);

  glGenBuffers(1, &lightDataUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, lightDataUBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(Lights), NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  //glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightDataUBO);
}

void Renderer::shutDown() noexcept
{
  glDeleteBuffers(1, &lightDataUBO);
}

void Renderer::render(Camera& camera) noexcept
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glm::mat4 viewMatrix;
  glm::mat4 projectionMatrix;
  glm::vec3 cameraPosition = glm::vec3(0.0f);

  for (auto it = meshInstances.begin(); it < meshInstances.end(); it++)
  {
    MeshInstance meshInstance = *it;
    meshInstance.getMaterial()->setUniforms(camera.getProjectionMatrix(), camera.getTransform().getViewMatrix(), glm::mat4(1.0), camera.getTransform().getLocalTranslation());
    glBindVertexArray(meshInstance.getMeshVAO());
		glDrawElements(GL_TRIANGLES, meshInstance.getMeshIndexCount(), GL_UNSIGNED_INT, nullptr);
  }

  // glBindBuffer(GL_UNIFORM_BUFFER, lightDataUBO);
  // glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Lights), nullptr);
  // glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

MeshInstance* Renderer::createMeshInstance(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
{
  meshInstances.emplace_back(mesh, material);
  return &meshInstances.back();
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
    std::cout << "textured" << std::endl;
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