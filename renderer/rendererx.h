#pragma once

#include <vector>
#include <array>
#include <glm/glm.hpp>
#include "material.h"
//#include "../Event/EventManager.hpp"
//#include "../World/ComponentTypes.hpp"
//#include "../World/TransformComponent.hpp"
//#include "../Animation/SkeletalMeshComponent.hpp"
//#include "StaticMeshComponent.hpp"
//#include "CameraComponent.hpp"
//#include "ShaderProgram.hpp"
//#include "DirectionalLightComponent.hpp"
//#include "SpotLightComponent.hpp"
//#include "PointLightComponent.hpp"
//#include "Material.hpp"
//#include "../DebugDrawing/DebugDrawingSystem.hpp"

class Renderer
{
public:
  static constexpr int NUM_HALF_MAX_DIRECTIONAL_LIGHTS = 1;
  static constexpr int NUM_HALF_MAX_POINT_LIGHTS = 3;
  static constexpr int NUM_HALF_MAX_SPOT_LIGHTS = 3;
  static constexpr int NUM_MAX_BONES = 70;

  Renderer() = default;

  void startUp() noexcept;
  void render(
        const InnerComponentHandle& cameraHandle,
        const glm::vec3& ambientLight,
        ComponentManager<StaticMeshComponent>& staticMeshDataManager,
        ComponentManager<SkeletalMeshComponent>& skeletalMeshDataManager,
        ComponentManager<TransformComponent> &transformDataManager,
        ComponentManager<CameraComponent> &cameraDataManager,
        ComponentManager<DirectionalLightComponent> &directionalLightDataManager,
        ComponentManager<SpotLightComponent> &spotLightDataManager,
        ComponentManager<PointLightComponent> &pointLightDataManager) noexcept;
  void shutDown() noexcept;
  std::shared_ptr<Material> createMaterial(MaterialType type, bool isForSkinning);

private:
  struct DirectionalLight
  {
    glm::vec3 colorIntensity; //12
    float padding04; //16
    glm::vec3 direction; //28
    float padding08; //32
  };

  struct PointLight
  {
    glm::vec3 colorIntensity; //12
    float padding04; //16
    glm::vec3 position; //28
    float maxRadius; //32
  };

  struct SpotLight
  {
    glm::vec3 colorIntensity; //12
    float maxRadius; //16
    glm::vec3 position; //28
    float cosPenumbraAngle; //32
    glm::vec3 direction;
    float cosUmbraAngle; //48
  };

  struct Lights
  {
    SpotLight spotLights[2 * NUM_HALF_MAX_SPOT_LIGHTS];
    PointLight pointLights[2 * NUM_HALF_MAX_POINT_LIGHTS]; 
    DirectionalLight directionalLights[2 * NUM_HALF_MAX_DIRECTIONAL_LIGHTS]; 
    glm::vec3 ambientLight; 
    int spotLightsCount; 
    int pointLightsCount; 
    int directionalLightsCount; 
  };

  std::array<ShaderProgram, 2 * static_cast<unsigned int>(MaterialType::MaterialTypeCount)> shaders;
  std::vector<glm::mat4> currentMatrixPalette;
  unsigned int lightDataUBO = 0;
};