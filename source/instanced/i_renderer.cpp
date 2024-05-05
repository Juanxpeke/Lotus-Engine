#include "i_renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../path_manager.h"
#include "../math/pairing.h"
#include "i_mesh.h"
#include "i_diffuse_flat_material.h"
#include "i_diffuse_textured_material.h"


void Renderer::startUp() noexcept
{
  shaders[static_cast<unsigned int>(MaterialType::DiffuseFlat)] = ShaderProgram(shaderPath("i_diffuse_flat.vert"), shaderPath("i_diffuse_flat.frag"));
  shaders[static_cast<unsigned int>(MaterialType::DiffuseTextured)] = ShaderProgram(shaderPath("i_diffuse_textured.vert"), shaderPath("i_diffuse_textured.frag"));

  glEnable(GL_DEPTH_TEST);

  glGenBuffers(1, &lightsDataBufferID);
  glBindBuffer(GL_UNIFORM_BUFFER, lightsDataBufferID);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(LightsData), NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightsDataBufferID);

  glfwSwapInterval(0);
}

void Renderer::shutDown() noexcept
{
  glDeleteBuffers(1, &lightsDataBufferID);
}

void Renderer::render(Camera& camera) noexcept
{
  cleanMeshInstances();

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
    lightsData.pointLights[i].radius = pointLight.getLightRadius();
  }

	glBindBuffer(GL_UNIFORM_BUFFER, lightsDataBufferID);
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

void Renderer::setAmbientLight(glm::vec3 color)
{
  ambientLight = color;
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

std::shared_ptr<MeshInstance> Renderer::createMeshInstance(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
{
  std::shared_ptr<GraphicsBatch> graphicsBatch = getGraphicsBatch(mesh, material);

  std::shared_ptr<MeshInstance> meshInstance = std::make_shared<MeshInstance>(mesh, material);

  meshInstances.push_back(meshInstance);
  graphicsBatch->addMeshInstance(meshInstance);

  return meshInstance;
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

std::shared_ptr<GraphicsBatch> Renderer::getGraphicsBatch(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
{
  std::shared_ptr<GraphicsBatch> graphicsBatch;
  uint64_t graphicsBatchID = LotusMath::szudzikPair(mesh->getVertexArrayID(), material->getShaderID());

  auto it = graphicsBatchMap.find(graphicsBatchID);

  if (it != graphicsBatchMap.end())
  {
    graphicsBatch = it->second;
  }
  else
  {
    std::cout << "Not found, creating new batch" << std::endl; // TODO: BETTER LOG
    
    GraphicsBatch* graphicsBatchPtr = new GraphicsBatch(mesh, material->getShaderID());

    graphicsBatch = std::shared_ptr<GraphicsBatch>(graphicsBatchPtr);
    graphicsBatchMap.insert({ graphicsBatchID, graphicsBatch });
  }

  return graphicsBatch;
}

void Renderer::cleanMeshInstances()
{
  for (auto meshInstance : meshInstances)
  {
    if (!meshInstance->hasDirtyMeshPtr() && !meshInstance->hasDirtyMaterialPtr()) { continue; }

    std::shared_ptr<Mesh> mesh = meshInstance->getMesh();
    std::shared_ptr<Material> material = meshInstance->getMaterial();
    std::shared_ptr<Mesh> oldMesh = meshInstance->getOldMesh();
    std::shared_ptr<Material> oldMaterial = meshInstance->getOldMaterial();

    std::shared_ptr<GraphicsBatch> graphicsBatch = getGraphicsBatch(mesh, material);
    std::shared_ptr<GraphicsBatch> oldGraphicsBatch = getGraphicsBatch(oldMesh, oldMaterial);

    // std::cout << "REMOVING FROM " << oldGraphicsBatch << std::endl;
    // std::cout << "ADDING TO " << graphicsBatch << std::endl;

    oldGraphicsBatch->removeMeshInstance(meshInstance);
    graphicsBatch->addMeshInstance(meshInstance);

    meshInstance->cleanDirtyMeshPtr();
    meshInstance->cleanDirtyMaterialPtr();
  }
}