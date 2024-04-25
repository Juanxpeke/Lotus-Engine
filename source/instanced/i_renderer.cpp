#include "i_renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../path_manager.h"
#include "i_mesh.h"
#include "i_diffuse_flat_material.h"
#include "i_diffuse_textured_material.h"


void Renderer::startUp() noexcept
{
  shaders[static_cast<unsigned int>(MaterialType::DiffuseFlat)] = ShaderProgram(shaderPath("i_diffuse_flat.vert"), shaderPath("i_diffuse_flat.frag"));
  shaders[static_cast<unsigned int>(MaterialType::DiffuseTextured)] = ShaderProgram(shaderPath("diffuse_textured.vert"), shaderPath("diffuse_textured.frag"));

  glEnable(GL_DEPTH_TEST);

  glGenBuffers(1, &lightsDataUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, lightsDataUBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(LightsData), NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightsDataUBO);

  glfwSwapInterval(0);
}

void Renderer::shutDown() noexcept
{
  glDeleteBuffers(1, &lightsDataUBO);
}

void Renderer::render(Camera& camera) noexcept
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glm::mat4 viewMatrix = camera.getViewMatrix();
  glm::mat4 projectionMatrix = camera.getProjectionMatrix();
  glm::vec3 cameraPosition = glm::vec3(0.0f);

  LightsData lightsData;

	lightsData.ambientLight = ambientLight;

  uint32_t directionalLightsCount = std::min(static_cast<uint32_t>(NUM_HALF_MAX_DIRECTIONAL_LIGHTS * 2), static_cast<uint32_t>(directionalLights.size()));
  lightsData.directionalLightsCount = static_cast<int>(directionalLightsCount);

  for (uint32_t i = 0; i < directionalLightsCount; i++)
  {
    const DirectionalLight& dirLight = directionalLights[i];
    lightsData.directionalLights[i].colorIntensity = dirLight.getLightColor() * dirLight.getLightIntensity();
    lightsData.directionalLights[i].direction = glm::rotate(dirLight.getLightDirection(), dirLight.getFrontVector());
  }

  uint32_t pointLightsCount = std::min(static_cast<uint32_t>(NUM_HALF_MAX_POINT_LIGHTS * 2), static_cast<uint32_t>(pointLights.size()));
  lightsData.pointLightsCount = static_cast<int>(pointLightsCount);

  for (uint32_t i = 0; i < pointLightsCount; i++)
  {
    const PointLight& pointLight = pointLights[i];
    lightsData.pointLights[i].colorIntensity = pointLight.getLightColor() * pointLight.getLightIntensity();
    lightsData.pointLights[i].position = pointLight.getLocalTranslation();
    lightsData.pointLights[i].radius = pointLight.getMaxRadius();
  }

	glBindBuffer(GL_UNIFORM_BUFFER, lightsDataUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightsData), &lightsData);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

  for (const auto& pair : graphicsBatchMap)
  {
    const std::shared_ptr<GraphicsBatch> graphicsBatch = pair.second;

    glUseProgram(graphicsBatch->getShaderID());

    graphicsBatch->updateBuffers();

    glUniformMatrix4fv(5, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(6, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glBindVertexArray(graphicsBatch->getMeshVAO());
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, graphicsBatch->getIndirectBufferID());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, GraphicsBatch::ModelsBindingPoint, graphicsBatch->getModelBufferID());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, GraphicsBatch::MaterialsBindingPoint, graphicsBatch->getMaterialBufferID());

    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*) 0, 1, 0);
  }
}

DirectionalLight* Renderer::createDirectionalLight()
{
  directionalLights.emplace_back();
  return &directionalLights.back();
}

PointLight* Renderer::createPointLight()
{
  pointLights.emplace_back();
  return &pointLights.back();
}

MeshInstance* Renderer::createMeshInstance(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
{
  std::shared_ptr<GraphicsBatch> graphicsBatch;

  auto it = graphicsBatchMap.find(mesh);

  if (it != graphicsBatchMap.end())
  {
    graphicsBatch = it->second;
  }
  else
  {
    std::cout << "Not found, creating new batch" << std::endl;
    GraphicsBatch* graphicsBatchPtr = new GraphicsBatch(mesh, material->getShaderID());
    graphicsBatch = std::shared_ptr<GraphicsBatch>(graphicsBatchPtr);
    graphicsBatchMap.insert({ mesh, graphicsBatch });
  }

  graphicsBatch->meshInstances.emplace_back(mesh, material);

  return &graphicsBatch->meshInstances.back();
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

void Renderer::setAmbientLight(glm::vec3 color)
{
  ambientLight = color;
}