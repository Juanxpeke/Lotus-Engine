#pragma once

#include <memory>
#include "procedural_data_generator.h"

namespace Lotus
{
  class Terrain
  {

  public:

    Terrain(const std::shared_ptr<ProceduralDataGenerator>& terrainDataGenerator) :
      dataGenerator(terrainDataGenerator),
      color(glm::vec3(1.0f, 1.0f, 1.0f))
    {}

    std::shared_ptr<ProceduralDataGenerator> getDataGenerator() const { return dataGenerator; }
    glm::vec3 getColor() const { return color; }

    void setDataGenerator(const std::shared_ptr<ProceduralDataGenerator>& terrainDataGenerator)
    {
      dataGenerator = terrainDataGenerator;
    }

    void setColor(const glm::vec3& terrainColor)
    {
      color = terrainColor;
    }

  private:

    std::shared_ptr<ProceduralDataGenerator> dataGenerator;
    glm::vec3 color;
  };
}