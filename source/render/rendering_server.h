#pragma once

#include <vector>
#include <memory>
#include "../scene/transform.h"
#include "../scene/camera.h"
#include "../lighting/light_manager.h"
#include "../terrain/terrain.h"

namespace Lotus
{

  class RenderingServer
  {
  public:
    RenderingServer(
        const std::shared_ptr<LightManager>& renderingLightManager,
        const std::shared_ptr<Terrain>& renderingTerrain);

    void startUp();
  
    void render(const Camera& camera);

  private:

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
      DirectionalLightData directionalLights[2 * 2];
      PointLightData pointLights[2 * 2];
      glm::vec3 ambientLight;
      int directionalLightsCount;
      int pointLightsCount;
    };
    
    unsigned int lightsDataBufferID = 0;
    std::shared_ptr<LightManager> lightManager;
    std::shared_ptr<Terrain> terrain;
  };

}