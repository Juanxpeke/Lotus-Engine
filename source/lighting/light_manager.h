#pragma once

#include <vector>
#include <memory>
#include "../math/types.h"
#include "directional_light.h"
#include "point_light.h"

namespace Lotus
{

  class LightManager
  {
  friend class RenderingServer;

  public:
    LightManager();

    void setAmbientLight(const glm::vec3& light);
    std::shared_ptr<DirectionalLight> createDirectionalLight();
    std::shared_ptr<PointLight> createPointLight();

  private:
    glm::vec3 ambientLight;
    std::vector<std::shared_ptr<DirectionalLight>> directionalLights;
    std::vector<std::shared_ptr<PointLight>> pointLights;

  };

}