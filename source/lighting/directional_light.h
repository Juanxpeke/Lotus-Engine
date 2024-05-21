#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../scene/node_3d.h"

namespace Lotus
{
  class DirectionalLight : public Node3D
  {
  public:

    DirectionalLight(
        const glm::vec3& lightColor = glm::vec3(1.0f),
        const glm::fquat& lightDirection = glm::fquat(1.0f, 0.0f, 0.0f, 0.0f)) :
      intensity(1.0f),
      color(lightColor),
      direction(lightDirection) {};

    float getLightIntensity() const { return intensity; }
    const glm::vec3& getLightColor() const { return color; }
    const glm::fquat& getLightDirection() const { return direction; }

    void setLightIntensity(const float& lightIntensity) { intensity = lightIntensity; }
    void setLightColor(const glm::vec3& lightColor) { color = lightColor; }
    void setLightDirection(const glm::fquat& lightDirection) { direction = lightDirection; }

  private:
    float intensity;
    glm::vec3 color;
    glm::fquat direction;
  };
}
