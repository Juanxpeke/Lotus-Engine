#pragma once

#include <random>

namespace Lotus
{
  class RNG
  {
  public:

    RNG(unsigned int seed = 0) : rng(seed) {}

    float getFloat()
    {
      std::uniform_real_distribution<float> dist(0.0f, 1.0f);
      return dist(rng);
    }

    float getFloatRange(float max)
    {
      getFloatRange(0, max);
    }

    float getFloatRange(float min, float max)
    {
      std::uniform_real_distribution<float> dist(min, max);
      return dist(rng);
    }

    int getInt()
    {
      std::uniform_int_distribution<int> dist;
      return dist(rng);
    }

    int getIntRange(int min, int max)
    {
      std::uniform_int_distribution<int> dist(min, max);
      return dist(rng);
    }

  private:
    std::mt19937 rng;
  };
}