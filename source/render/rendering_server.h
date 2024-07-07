#pragma once

#include <vector>
#include <memory>
#include "../scene/transform.h"
#include "../scene/camera.h"
#include "../lighting/light_manager.h"
#include "../terrain/terrain_renderer.h"
#include "gpu_buffer.h"
#include "indirect/indirect_object_renderer.h"

namespace Lotus
{

  class RenderingServer
  {
  public:
    RenderingServer(
        const std::shared_ptr<LightManager>& renderingLightManager,
        const std::shared_ptr<IndirectObjectRenderer>& renderingIndirectScene,
        const std::shared_ptr<TerrainRenderer>& renderingTerrain);

    void startUp();
  
    void render(const Camera& camera);

  private:

    // Buffer filling
    void fillCameraBuffer(const Camera& camera);    
    void fillLightsBuffer();

    // Rendering
    void renderTraditionalObjects();
    void renderIndirectObjects();
    void renderTerrain(const Camera& camera);

    struct DirectionalLightData
    {
      glm::vec3 colorIntensity;
      float padding04;
      glm::vec3 direction;
      float padding08;
    };

    struct PointLightData
    {
      glm::vec3 colorIntensity;
      float padding04;
      glm::vec3 position;
      float radius;
    };

    struct SpotLightData
    {
      glm::vec3 colorIntensity;
      float radius;
      glm::vec3 position;
      float cosPenumbraAngle;
      glm::vec3 direction;
      float cosUmbraAngle;
    };

    struct LightsData
    {
      DirectionalLightData directionalLights[2];
      PointLightData pointLights[2];
      glm::vec3 ambientLight;
      int directionalLightsCount;
      int pointLightsCount;
    };

    struct CameraData
    {
      glm::mat4 view;
      glm::mat4 projection;
      glm::mat4 viewProjection;
      glm::vec3 cameraPosition;
      float padding04;
    };
    
    UniformBuffer<LightsData> lightsBuffer;
    UniformBuffer<CameraData> cameraBuffer;

    std::shared_ptr<LightManager> lightManager;
    std::shared_ptr<IndirectObjectRenderer> indirectScene;
    std::shared_ptr<TerrainRenderer> terrain;
  };

}