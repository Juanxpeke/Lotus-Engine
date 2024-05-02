#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../scene/node_3d.h"

class PointLight : public Node3D
{
public:

  PointLight(
      const glm::vec3& lightColor = glm::vec3(1.0f),
      float radius = 10.0f) :
    intensity(1.0f),
    color(lightColor),
    radius(radius) {};

  float getLightIntensity() const { return intensity; }
  const glm::vec3& getLightColor() const { return color; }
  float getLightRadius() const { return radius; }

  void setLightIntensity(const float& lightIntensity) { intensity = lightIntensity; }
  void setLightColor(const glm::vec3& lightColor) { color = lightColor; }
  void setLightRadius(float lightRadius) { radius = lightRadius; }

private:
  float intensity;
  glm::vec3 color;
  float radius;
};