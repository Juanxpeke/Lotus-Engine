#pragma once

#include <cstdint>
#include <algorithm>
#include <memory>
#include "PerlinNoise.hpp"

namespace Lotus
{
  class TerrainData
  {
  public:
    TerrainData(
        int initialWidth,
        int initialHeight,
        double initialFrequency = 8.0,
        int initialOctaves = 8) :
      width(initialWidth),
      height(initialHeight),
      frequency(initialFrequency),
      octaves(initialOctaves)
    {
      data = std::make_unique<float[]>(width * height);

      frequency = std::clamp(frequency, 0.1, 64.0);
      octaves = std::clamp(octaves, 1, 16);

      uint32_t seed = 0;
      const siv::PerlinNoise perlin{ seed };
      const double fx = (frequency / width);
      const double fy = (frequency / height);

      for (int y = 0; y < height; ++y)
      {
        for (int x = 0; x < width; ++x)
        {
          data[y * width + x] = perlin.octave2D_01((x * fx), (y * fy), octaves) * 4.0f;
        }
      }
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

    std::unique_ptr<float[]> data;
  };
}

