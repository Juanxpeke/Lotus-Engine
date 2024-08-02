#pragma once

#include <vector>
#include <memory>
#include "../scene/transform.h"
#include "../scene/camera.h"
#include "../lighting/directional_light.h"
#include "../lighting/point_light.h"
#include "../terrain/terrain_renderer.h"
#include "gpu_structures.h"
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

    /* Modes */
    void setRenderingMode(RenderingMode renderingMode);
    void switchRenderingMode();
    
    /* Lighting */
    void setAmbientLight(const glm::vec3& light);
    std::shared_ptr<DirectionalLight> createDirectionalLight();
    std::shared_ptr<PointLight> createPointLight();
    
    /* Objects */
    void setDefaultObjectRenderingMethod(RenderingMethod renderingMethod);
    std::shared_ptr<MeshObject> createObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
    std::shared_ptr<MeshObject> createObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, RenderingMethod renderingMethod);
    std::shared_ptr<Material> createMaterial(MaterialType type);

    /* Terrain */
    void setDefaultTerrainRenderingMethod(RenderingMethod renderingMethod);
    void setTerrainLevels(uint32_t levels);
    void setTerrainTileResolution(uint32_t tileResolution);
    std::shared_ptr<Terrain> createTerrain(const std::shared_ptr<ProceduralDataGenerator>& terrainDataGenerator);

  private:

    void fillCameraBuffer(const Camera& camera);    
    void fillLightsBuffer();
   
    RenderingMode mode;
    RenderingMethod defaultObjectRenderingMethod;
    RenderingMethod defaultTerrainRenderingMethod;

    glm::vec3 ambientLight;
    std::vector<std::shared_ptr<DirectionalLight>> directionalLights;
    std::vector<std::shared_ptr<PointLight>> pointLights;

    UniformBuffer<GPULightsData> lightsBuffer;
    UniformBuffer<GPUCameraData> cameraBuffer;

    TraditionalObjectRenderer traditionalObjectRenderer;
    IndirectObjectRenderer indirectObjectRenderer;
    TerrainRenderer terrainRenderer;
  };

}