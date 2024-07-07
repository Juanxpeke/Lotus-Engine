#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "../math/randomizer.h"
#include "../render/gpu_mesh.h"
#include "../render/indirect/indirect_object_renderer.h"
#include "procedural_data_generator.h"

namespace Lotus
{
  
  class ObjectPlacer
  {
  public:
    ObjectPlacer(
        const std::shared_ptr<ProceduralDataGenerator>& dataGenerator,
        const std::shared_ptr<IndirectObjectRenderer>& indirectScene,
        float radius,
        uint8_t samplesBeforeRejection = 30,
        uint32_t seed = 0);

    void initialize();

    void addObject(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material, float weight);

    void update(bool forced = false);

  private:
    
    struct ObjectPlacerItem
    {
      std::shared_ptr<Mesh> mesh;
      std::shared_ptr<Material> material;
    };

    void generateObjects(const glm::ivec2& chunk);
    void generateObjects(int x, int y);

    float radius;
    uint8_t samplesBeforeRejection;
    Randomizer randomizer;
    std::shared_ptr<ProceduralDataGenerator> dataGenerator;

    std::vector<ObjectPlacerItem> objectItemsPool;

    std::shared_ptr<IndirectObjectRenderer> scene;

    bool initialized;
  };

}