#pragma once

#include <cstdint>
#include <memory>
#include <algorithm>
#include "linear_algebra.h"
#include "PerlinNoise.hpp"

namespace Lotus
{

  class Perlin2DArray
  {
  public:

    static void fill(
      float* destination,
      int width,
      int height,
      Vec2i offset = { 0, 0 },
      double frequency = 8.0,
      int octaves = 8,
      uint32_t seed = 0)
    {
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

          destination[y * width + x] = perlin.noise2D_01(sampleX, sampleY);
        }
      }
    }
  };

}