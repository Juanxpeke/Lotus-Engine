#pragma once

#include <vector>
#include <array>
#include <glm/glm.hpp>
#include "../scene/transform.h"
#include "../scene/camera.h"
#include "directional_light.h"
#include "shader_program.h"
#include "material.h"
#include "mesh_instance.h"

class Renderer
{
public:
  static constexpr int NUM_HALF_MAX_DIRECTIONAL_LIGHTS = 1;
  static constexpr int NUM_HALF_MAX_POINT_LIGHTS = 3;
  static constexpr int NUM_HALF_MAX_SPOT_LIGHTS = 3;
  static constexpr int NUM_MAX_BONES = 70;

  Renderer() = default;

  void startUp() noexcept;

  void shutDown() noexcept;

  void render(Camera& camera) noexcept;

  MeshInstance* createMeshInstance(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

  std::shared_ptr<Material> createMaterial(MaterialType type);

  void setAmbientLight(glm::vec3 color);

private:
  struct DirectionalLightData
  {
    glm::vec3 color;      // 12
    float padding04;      // 16
    glm::vec3 direction;  // 28
    float padding08;      // 32
  };

  struct PointLightData
  {
    glm::vec3 color;    // 12
    float padding04;    // 16
    glm::vec3 position; // 28
    float maxRadius;    // 32
  };

  struct SpotLightData
  {
    glm::vec3 color;        // 12
    float maxRadius;        // 16
    glm::vec3 position;     // 28
    float cosPenumbraAngle; // 32
    glm::vec3 direction;    // 44
    float cosUmbraAngle;    // 48
  };

  struct LightsData
  {
    DirectionalLightData directionalLights[2 * NUM_HALF_MAX_DIRECTIONAL_LIGHTS]; 
    glm::vec3 ambientLight;
    int directionalLightsCount;
  };

  std::array<ShaderProgram, static_cast<unsigned int>(MaterialType::MaterialTypeCount)> shaders;

  std::vector<MeshInstance> meshInstances;
  std::vector<DirectionalLight> directionalLights;
  
  glm::vec3 ambientLight;

  unsigned int lightsDataUBO = 0;
};