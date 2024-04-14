#pragma once

#include <vector>
#include <array>
#include <glm/glm.hpp>
#include "../scene/transform.h"
#include "../scene/camera.h"
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


  std::vector<MeshInstance> meshInstances;
private:
  struct DirectionalLight
  {
    glm::vec3 colorIntensity; //12
    float padding04; //16
    glm::vec3 direction; //28
    float padding08; //32
  };

  struct PointLight {
    glm::vec3 colorIntensity; //12
    float padding04; //16
    glm::vec3 position; //28
    float maxRadius; //32
  };

  struct SpotLight {
    glm::vec3 colorIntensity; //12
    float maxRadius; //16
    glm::vec3 position; //28
    float cosPenumbraAngle; //32
    glm::vec3 direction;
    float cosUmbraAngle; //48
  };

  struct Lights {
    SpotLight spotLights[2 * NUM_HALF_MAX_SPOT_LIGHTS];
    PointLight pointLights[2 * NUM_HALF_MAX_POINT_LIGHTS]; 
    DirectionalLight directionalLights[2 * NUM_HALF_MAX_DIRECTIONAL_LIGHTS]; 
    glm::vec3 ambientLight; 
    int spotLightsCount; 
    int pointLightsCount; 
    int directionalLightsCount; 
  };

  std::array<ShaderProgram, static_cast<unsigned int>(MaterialType::MaterialTypeCount)> shaders;

  
  unsigned int lightDataUBO = 0;

};