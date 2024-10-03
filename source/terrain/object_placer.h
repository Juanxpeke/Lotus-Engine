#pragma once

#include <memory>
#include <vector>
#include "../math/types.h"
#include "../math/randomizer.h"
#include "../render/gpu_mesh.h"
#include "../render/rendering_server.h"
#include "procedural_data_generator.h"

namespace Lotus
{
  
  class ObjectPlacer
  {
  public:
    ObjectPlacer(
        const std::shared_ptr<ProceduralDataGenerator>& placerDataGenerator,
        RenderingServer* placerRenderingServer,
        RenderingMethod placerRenderingMethod,
        float placerRadius,
        uint8_t placerSamplesBeforeRejection = 30,
        uint32_t seed = 0);

    void initialize();

    void addObject(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material, bool randomScale = false);

    void update(bool forced = false);

    uint32_t getChunksLoaded() { return chunksLoaded; }

  private:
    
    struct ObjectPlacerItem
    {
      std::shared_ptr<Mesh> mesh;
      std::shared_ptr<Material> material;
      bool randomScale;
    };

    void generateObjects(const glm::ivec2& chunk);
    void generateObjects(int x, int y);

    float radius;
    uint8_t samplesBeforeRejection;
    Randomizer randomizer;
    std::shared_ptr<ProceduralDataGenerator> dataGenerator;

    std::vector<ObjectPlacerItem> objectItemsPool;

    RenderingServer* renderingServer;
    RenderingMethod renderingMethod;

    bool initialized;

    uint32_t chunksLoaded;
  };

}