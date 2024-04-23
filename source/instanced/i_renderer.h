#pragma once

#include <vector>
#include <array>
#include <unordered_map>
#include <glm/glm.hpp>
#include "../scene/transform.h"
#include "../scene/camera.h"
#include "../lighting/directional_light.h"
#include "../lighting/point_light.h"
#include "i_shader_program.h"
#include "i_material.h"
#include "graphics_batch.h"

class Renderer
{
public:
  static constexpr int NUM_HALF_MAX_DIRECTIONAL_LIGHTS = 1;
  static constexpr int NUM_HALF_MAX_POINT_LIGHTS = 1;
  static constexpr int NUM_HALF_MAX_SPOT_LIGHTS = 1;

  Renderer() = default;

  void startUp() noexcept;
  void shutDown() noexcept;

  void render(Camera& camera) noexcept;

  void setAmbientLight(glm::vec3 color);
  DirectionalLight* createDirectionalLight();
  PointLight* createPointLight();

  MeshInstance* createMeshInstance(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

  std::shared_ptr<Material> createMaterial(MaterialType type);

private:
  struct DirectionalLightData
  {
    glm::vec3 colorIntensity; // 12
    float padding04;          // 16
    glm::vec3 direction;      // 28
    float padding08;          // 32
  };

  struct PointLightData
  {
    glm::vec3 colorIntensity; // 12
    float padding04;          // 16
    glm::vec3 position;       // 28
    float radius;             // 32
  };

  struct SpotLightData
  {
    glm::vec3 colorIntensity; // 12
    float radius;             // 16
    glm::vec3 position;       // 28
    float cosPenumbraAngle;   // 32
    glm::vec3 direction;      // 44
    float cosUmbraAngle;      // 48
  };

  struct LightsData
  {
    DirectionalLightData directionalLights[2 * NUM_HALF_MAX_DIRECTIONAL_LIGHTS];
    PointLightData pointLights[2 * NUM_HALF_MAX_POINT_LIGHTS];
    glm::vec3 ambientLight;
    int directionalLightsCount;
    int pointLightsCount;
  };

  std::array<ShaderProgram, static_cast<unsigned int>(MaterialType::MaterialTypeCount)> shaders;

  unsigned int lightsDataUBO = 0;
  glm::vec3 ambientLight;
  std::vector<DirectionalLight> directionalLights;
  std::vector<PointLight> pointLights;

  std::unordered_map<std::shared_ptr<Mesh>, std::shared_ptr<GraphicsBatch>> graphicsBatchMap;
};