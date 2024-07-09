#pragma once

#include <vector>
#include <memory>
#include "../scene/transform.h"
#include "../scene/camera.h"
#include "../lighting/light_manager.h"
#include "../terrain/terrain_renderer.h"
#include "gpu_buffer.h"
#include "material.h"
#include "unlit_flat_material.h"
#include "diffuse_flat_material.h"
#include "diffuse_textured_material.h"
#include "traditional/traditional_object_renderer.h"
#include "indirect/indirect_object_renderer.h"

namespace Lotus
{
  enum class RenderingMode
  {
    Fill,
    Wireframe
  };

  enum class RenderingMethod
  {
    Traditional,
    Indirect
  };

  class RenderingServer
  {
  public:
    RenderingServer();

    void startUp();
  
    void render(const Camera& camera);

    // Modes
    void setRenderingMode(RenderingMode renderingMode);
    void switchRenderingMode();

    // Lighting
    void setAmbientLight(const glm::vec3& light);
    std::shared_ptr<DirectionalLight> createDirectionalLight();
    std::shared_ptr<PointLight> createPointLight();

    // Traditional Objects

    // Indirect Objects
    std::shared_ptr<MeshObject> createObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, RenderingMethod method = RenderingMethod::Indirect);
    std::shared_ptr<Material> createMaterial(MaterialType type);

    // Terrain
    void setTerrainLevels(uint32_t levels);
    void setTerrainTileResolution(uint32_t tileResolution);
    void setTerrain(Terrain* terrain);

  private:

    // Buffers
    void fillCameraBuffer(const Camera& camera);    
    void fillLightsBuffer();

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
    
    RenderingMode mode;

    UniformBuffer<LightsData> lightsBuffer;
    UniformBuffer<CameraData> cameraBuffer;

    LightManager lightManager;
    TraditionalObjectRenderer traditionalObjectRenderer;
    IndirectObjectRenderer indirectObjectRenderer;
    TerrainRenderer terrainRenderer;
  };

}