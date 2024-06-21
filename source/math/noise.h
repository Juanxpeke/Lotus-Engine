#pragma once

#include <cstdint>
#include <memory>
#include <algorithm>
#include <glm/glm.hpp>
#include "PerlinNoise.hpp"

namespace Lotus
{

  class Perlin2DArray
  {
  public:
    Perlin2DArray(
        int dataWidth,
        int dataHeight,
        glm::vec2 dataOffset = { 0, 0 },
        double dataFrequency = 8.0,
        int dataOctaves = 8,
        uint32_t seed = 0) :
      width(dataWidth),
      height(dataHeight),
      frequency(dataFrequency),
      octaves(dataOctaves),
      offset(dataOffset)
    {
      data = new float[width * height];

      frequency = std::clamp(frequency, 0.1, 64.0);
      octaves = std::clamp(octaves, 1, 16);

      const siv::PerlinNoise perlin(seed);
      const double fx = (frequency / width);
      const double fy = (frequency / height);

      for (int y = 0; y < height; ++y)
      {
        for (int x = 0; x < width; ++x)
        {
          float sampleX = (x * fx) + offset.x * fx;
          float sampleY = (y * fy) + offset.y * fy;

          data[y * width + x] = perlin.noise2D_01(sampleX, sampleY);//perlin.normalizedOctave2D_01(sampleX, sampleY, octaves);
        }
      }
    }

    ~Perlin2DArray()
    {
      delete[] data;
    }

    const float* getData() const
    {
      return data;
    }

    float get(int x, int y)
    {
      return data[y * width + x];
    }

  private:
    int width;
    int height;
    double frequency;
    int octaves;
    glm::vec2 offset;

    float* data;
  };

}