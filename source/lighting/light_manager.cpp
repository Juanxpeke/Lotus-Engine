#include "light_manager.h"

namespace Lotus
{

  void LightManager::setAmbientLight(const glm::vec3& light)
  {
    ambientLight = light;
  }

  std::shared_ptr<DirectionalLight> LightManager::createDirectionalLight()
  {
    std::shared_ptr<DirectionalLight> directionalLight = std::make_shared<DirectionalLight>();
    directionalLights.push_back(directionalLight);

    return directionalLight;
  }

  std::shared_ptr<PointLight> LightManager::createPointLight()
  {
    std::shared_ptr<PointLight> pointLight = std::make_shared<PointLight>();
    pointLights.push_back(pointLight);

    return pointLight;
  }

  void LightManager::removeDirectionalLight(const std::shared_ptr<DirectionalLight>& directionalLight)
  {

  }

  void LightManager::removePointLight(const std::shared_ptr<PointLight>& pointLight)
  {
    
  }

}