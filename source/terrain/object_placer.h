#pragma once

#include <memory>
#include <vector>
#include "../math/linear_algebra.h"
#include "../math/randomizer.h"
#include "../render/gpu_mesh.h"
#include "procedural_data_generator.h"

namespace Lotus
{

  class ObjectPlacer
  {
  public:
    ObjectPlacer(const std::shared_ptr<ProceduralDataGenerator>& heightsGenerator, float radius, uint8_t samplesBeforeRejection = 30, uint32_t seed = 0);

    void addObject();

  private:

    void generateObjects(const Vec2i& chunk);
    void generateObjects(int x, int y);

    float radius;
    uint8_t samplesBeforeRejection;
    Randomizer randomizer;
    std::shared_ptr<ProceduralDataGenerator> heightsGenerator;

    std::vector<std::shared_ptr<GPUMesh>> meshesPool;
    std::vector<float> meshesWeights;

  };

}