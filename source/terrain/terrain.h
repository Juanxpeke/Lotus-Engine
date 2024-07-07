#pragma once

#include <memory>
#include "procedural_data_generator.h"

namespace Lotus
{
  class Terrain
  {

  public:

    Terrain(const std::shared_ptr<ProceduralDataGenerator>& terrainDataGenerator) :
      dataGenerator(terrainDataGenerator)
    {}

    std::shared_ptr<ProceduralDataGenerator> getDataGenerator() const { return dataGenerator; }

    void setDataGenerator(const std::shared_ptr<ProceduralDataGenerator>& terrainDataGenerator)
    {
      dataGenerator = terrainDataGenerator;
    }

  private:

    std::shared_ptr<ProceduralDataGenerator> dataGenerator;
  };
}