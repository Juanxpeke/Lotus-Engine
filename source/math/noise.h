#pragma once

#include <cstdint>
#include <memory>
#include <algorithm>
#include "linear_algebra.h"
#include "PerlinNoise.hpp"

namespace Lotus
{

  struct PerlinNoiseConfig
  {
    uint32_t seed = 0;
    double frequency = 8.0;
    int octaves = 8;
    Vec2i offset = { 0, 0 };
  };

  class Perlin2DArray
  {
  public:

    static void fill(
        float* destination,
        int width,
        int height,
        const PerlinNoiseConfig& noiseConfig)
    {
      double frequency = std::clamp(noiseConfig.frequency, 0.1, 64.0);
      int octaves = std::clamp(noiseConfig.octaves, 1, 16);

      const siv::PerlinNoise perlin(noiseConfig.seed);
      
      const double fx = (frequency / width);
      const double fy = (frequency / height);

      for (int y = 0; y < height; ++y)
      {
        for (int x = 0; x < width; ++x)
        {
          float xSample = (x * fx) + noiseConfig.offset.x * fx;
          float ySample = (y * fy) + noiseConfig.offset.y * fy;

          destination[y * width + x] = perlin.noise2D_01(xSample, ySample);
        }
      }
    }
  };

}