#include "rendering_server.h"

#include <algorithm>
#include <glad/glad.h>
#include "identifiers.h"

namespace Lotus
{

  RenderingServer::RenderingServer()
  {
    cameraBuffer.allocate();
    cameraBuffer.setBindingPoint(CameraBufferBindingPoint);

    lightsBuffer.allocate();
    lightsBuffer.setBindingPoint(LightsBufferBindingPoint);
  }

  void RenderingServer::startUp()
  {
    glEnable(GL_DEPTH_TEST);
    
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);
  
  }

  void RenderingServer::render(const Camera& camera)
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    fillCameraBuffer(camera);
    fillLightsBuffer();

    cameraBuffer.bind();
    lightsBuffer.bind();

    indirectObjectRenderer.render();
    terrainRenderer.render(camera);

    lightsBuffer.unbind();
    cameraBuffer.unbind();
  }

  void RenderingServer::setAmbientLight(const glm::vec3& light)
  {
    lightManager.setAmbientLight(light);
  }
  
  std::shared_ptr<DirectionalLight> RenderingServer::createDirectionalLight()
  {
    return lightManager.createDirectionalLight();
  }
  
  std::shared_ptr<PointLight> RenderingServer::createPointLight()
  {
    return lightManager.createPointLight();
  }

  std::shared_ptr<MeshInstance> RenderingServer::createObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
  {
    return indirectObjectRenderer.createObject(mesh, material);
  }

  std::shared_ptr<Material> RenderingServer::createMaterial(MaterialType type)
  {
    return indirectObjectRenderer.createMaterial(type);
  }



  void RenderingServer::setTerrain(Terrain* terrain)
  {
    terrainRenderer.setTerrain(terrain);
  }

  void RenderingServer::fillCameraBuffer(const Camera& camera)
  {
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    glm::vec3 cameraPosition = camera.getLocalTranslation();

    CameraData cameraData;

    cameraData.view = camera.getViewMatrix();
    cameraData.projection = camera.getProjectionMatrix();
    cameraData.viewProjection = cameraData.view * cameraData.projection;
    cameraData.cameraPosition = camera.getLocalTranslation();

    cameraBuffer.write(&cameraData);
  }

  void RenderingServer::fillLightsBuffer()
  {
    LightsData lightsData;

    lightsData.ambientLight = lightManager.ambientLight;

    uint32_t directionalLightsCount = std::min(static_cast<uint32_t>(2 * 2), static_cast<uint32_t>(lightManager.directionalLights.size()));
    lightsData.directionalLightsCount = static_cast<int>(directionalLightsCount);

    for (uint32_t i = 0; i < directionalLightsCount; i++)
    {
      const std::shared_ptr<DirectionalLight>& dirLight = lightManager.directionalLights[i];
      lightsData.directionalLights[i].colorIntensity = dirLight->getLightColor() * dirLight->getLightIntensity();
      lightsData.directionalLights[i].direction = glm::rotate(dirLight->getLightDirection(), dirLight->getFrontVector());
    }

    uint32_t pointLightsCount = std::min(static_cast<uint32_t>(2 * 2), static_cast<uint32_t>(lightManager.pointLights.size()));
    lightsData.pointLightsCount = static_cast<int>(pointLightsCount);

    for (uint32_t i = 0; i < pointLightsCount; i++)
    {
      const std::shared_ptr<PointLight>& pointLight = lightManager.pointLights[i];
      lightsData.pointLights[i].colorIntensity = pointLight->getLightColor() * pointLight->getLightIntensity();
      lightsData.pointLights[i].position = pointLight->getLocalTranslation();
      lightsData.pointLights[i].radius = pointLight->getLightRadius();
    }

    lightsBuffer.write(&lightsData);
  }

}