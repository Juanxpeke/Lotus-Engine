#pragma once

#include <vector>
#include <memory>
#include "../scene/transform.h"
#include "../scene/camera.h"
#include "../lighting/light_manager.h"
#include "../render/indirect/indirect_scene.h"
#include "../terrain/terrain.h"

namespace Lotus
{

  class RenderingServer
  {
  public:
    RenderingServer(
        const std::shared_ptr<LightManager>& renderingLightManager,
        const std::shared_ptr<IndirectScene>& renderingIndirectScene,
        const std::shared_ptr<Terrain>& renderingTerrain);

    void startUp();
  
    void render(const Camera& camera);

  private:
    static constexpr unsigned int CameraBufferBindingPoint = 0;
    static constexpr unsigned int LightsBufferBindingPoint = 1;

    void renderLights(const Camera& camera);
    void renderTraditionalScene(const Camera& camera);
    void renderIndirectScene(const Camera& camera);
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
    
    unsigned int lightsDataBufferID = 0;
    unsigned int cameraDataBufferID = 0;

    std::shared_ptr<LightManager> lightManager;
    std::shared_ptr<IndirectScene> indirectScene;
    std::shared_ptr<Terrain> terrain;
  };

}