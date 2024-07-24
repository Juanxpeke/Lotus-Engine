#include "rendering_server.h"

#include <algorithm>
#include "../util/opengl_entry.h"
#include "../util/profile.h"
#include "identifiers.h"

namespace Lotus
{

  RenderingServer::RenderingServer()
  {}

  void RenderingServer::startUp()
  {
    glEnable(GL_DEPTH_TEST);
    
    mode = RenderingMode::Fill;
    
    cameraBuffer.allocate();
    cameraBuffer.setBindingPoint(CameraBufferBindingPoint);

    lightsBuffer.allocate();
    lightsBuffer.setBindingPoint(LightsBufferBindingPoint);
  }

  void RenderingServer::render(const Camera& camera)
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    fillCameraBuffer(camera);
    fillLightsBuffer();

    cameraBuffer.bind();
    lightsBuffer.bind();

    traditionalObjectRenderer.render();
    indirectObjectRenderer.render();
    terrainRenderer.render(camera);

    lightsBuffer.unbind();
    cameraBuffer.unbind();

    LOTUS_PROFILE_END_FRAME();
  }

  void RenderingServer::setRenderingMode(RenderingMode renderingMode)
  {
    mode = renderingMode;

    switch (mode)
    {
      case RenderingMode::Fill:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_POLYGON_OFFSET_LINE);
        break;
      case RenderingMode::Wireframe:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-1, -1);
        break;
    }
  }

  void RenderingServer::switchRenderingMode()
  {
    switch (mode)
    {
      case RenderingMode::Fill:
        setRenderingMode(RenderingMode::Wireframe);
        break;
      case RenderingMode::Wireframe:
        setRenderingMode(RenderingMode::Fill);
        break;
    }
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

  std::shared_ptr<MeshObject> RenderingServer::createObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, RenderingMethod method)
  {
    switch(method)
    {
      case RenderingMethod::Traditional:
        return traditionalObjectRenderer.createObject(mesh, material);
      case RenderingMethod::Indirect:
        return indirectObjectRenderer.createObject(mesh, material);
      default:
        return indirectObjectRenderer.createObject(mesh, material);
    }
  }

  std::shared_ptr<Material> RenderingServer::createMaterial(MaterialType type)
  {
    switch (type)
    {
      case MaterialType::UnlitFlat:
        return std::make_shared<UnlitFlatMaterial>();
      case MaterialType::UnlitTextured:
        return std::make_shared<UnlitFlatMaterial>();
      case MaterialType::DiffuseFlat:
        return std::make_shared<DiffuseFlatMaterial>();
      case MaterialType::DiffuseTextured:
        return std::make_shared<DiffuseTexturedMaterial>();
      case MaterialType::MaterialTypeCount:
        return std::make_shared<DiffuseFlatMaterial>();
      default:
        return nullptr;
    }
  }

  void RenderingServer::setTerrainLevels(uint32_t levels)
  {
    terrainRenderer.setLevels(levels);
  }

  void RenderingServer::setTerrainTileResolution(uint32_t tileResolution)
  {
    terrainRenderer.setTileResolution(tileResolution);
  }

  std::shared_ptr<Terrain> RenderingServer::createTerrain(const std::shared_ptr<ProceduralDataGenerator>& terrainDataGenerator)
  {
    return terrainRenderer.createTerrain(terrainDataGenerator);
  }

  void RenderingServer::fillCameraBuffer(const Camera& camera)
  {
    CameraData cameraData;

    cameraData.view = camera.getViewMatrix();
    cameraData.projection = camera.getProjectionMatrix();
    cameraData.viewProjection = cameraData.projection * cameraData.view;
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