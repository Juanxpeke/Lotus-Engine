#include "renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../util/path_manager.h"
#include "diffuse_flat_material.h"
#include "diffuse_textured_material.h"

namespace Lotus
{
  namespace Traditional
  {
    void Renderer::startUp() noexcept
    {
      shaders[static_cast<unsigned int>(MaterialType::DiffuseFlat)] = ShaderProgram(shaderPath("traditional/diffuse_flat.vert"), shaderPath("traditional/diffuse_flat.frag"));
      shaders[static_cast<unsigned int>(MaterialType::DiffuseTextured)] = ShaderProgram(shaderPath("traditional/diffuse_textured.vert"), shaderPath("traditional/diffuse_textured.frag"));

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
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      glm::mat4 viewMatrix = camera.getViewMatrix();
      glm::mat4 projectionMatrix = camera.getProjectionMatrix();
      glm::vec3 cameraPosition = camera.getLocalTranslation();

      LightsData lightsData;

      lightsData.ambientLight = ambientLight;

      uint32_t directionalLightsCount = std::min(static_cast<uint32_t>(NUM_HALF_MAX_DIRECTIONAL_LIGHTS * 2), static_cast<uint32_t>(directionalLights.size()));
      lightsData.directionalLightsCount = static_cast<int>(directionalLightsCount);

      for (uint32_t i = 0; i < directionalLightsCount; i++)
      {
        const std::shared_ptr<DirectionalLight>& dirLight = directionalLights[i];
        lightsData.directionalLights[i].colorIntensity = dirLight->getLightColor() * dirLight->getLightIntensity();
        lightsData.directionalLights[i].direction = glm::rotate(dirLight->getLightDirection(), dirLight->getFrontVector());
      }

      uint32_t pointLightsCount = std::min(static_cast<uint32_t>(NUM_HALF_MAX_POINT_LIGHTS * 2), static_cast<uint32_t>(pointLights.size()));
      lightsData.pointLightsCount = static_cast<int>(pointLightsCount);

      for (uint32_t i = 0; i < pointLightsCount; i++)
      {
        const std::shared_ptr<PointLight>& pointLight = pointLights[i];
        lightsData.pointLights[i].colorIntensity = pointLight->getLightColor() * pointLight->getLightIntensity();
        lightsData.pointLights[i].position = pointLight->getLocalTranslation();
        lightsData.pointLights[i].radius = pointLight->getLightRadius();
      }

      glBindBuffer(GL_UNIFORM_BUFFER, lightsDataBufferID);
      glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightsData), &lightsData);
      glBindBuffer(GL_UNIFORM_BUFFER, 0);

      for (int i = 0; i < meshInstances.size(); i++)
      {
        const std::shared_ptr<MeshInstance>& meshInstance = meshInstances[i];
        const std::shared_ptr<GPUMesh>& mesh = meshInstance->getMesh();

        meshInstance->getMaterial()->setUniforms(projectionMatrix, viewMatrix, meshInstance->getModelMatrix(), cameraPosition);
        glBindVertexArray(mesh->getVertexArrayID());
        glDrawElements(GL_TRIANGLES, mesh->getIndicesCount(), GL_UNSIGNED_INT, nullptr);
      }
    }

    std::shared_ptr<MeshInstance> Renderer::createMeshInstance(std::shared_ptr<GPUMesh> mesh, std::shared_ptr<Material> material)
    {
      std::shared_ptr<MeshInstance> meshInstance = std::make_shared<MeshInstance>(mesh, material);

      meshInstances.push_back(meshInstance);

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
  
    std::shared_ptr<DirectionalLight> Renderer::createDirectionalLight()
    {
      std::shared_ptr<DirectionalLight> directionalLight = std::make_shared<DirectionalLight>();
      directionalLights.push_back(directionalLight);

      return directionalLight;
    }

    std::shared_ptr<PointLight> Renderer::createPointLight()
    {
      std::shared_ptr<PointLight> pointLight = std::make_shared<PointLight>();
      pointLights.push_back(pointLight);

      return pointLight;
    }

    void Renderer::setAmbientLight(glm::vec3 color)
    {
      ambientLight = color;
    }
  }
}