#pragma once

#include <random>

namespace Lotus
{
  class Randomizer
  {
  public:

    Randomizer(unsigned int seed = 0) : rng(seed) {}

    float getFloat()
    {
      std::uniform_real_distribution<float> dist(0.0f, 1.0f);
      return dist(rng);
    }

    float getFloatRange(float max)
    {
      return getFloatRange(0, max);
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

    int getIntRange(int max)
    {
      return getIntRange(0, max);
    }

    int getIntRange(int min, int max)
    {
      std::uniform_int_distribution<int> dist(min, max);
      return dist(rng);
    }

    bool getBool()
    {
      return getIntRange(0, 1) ? true : false;
    }

  private:
    std::mt19937 rng;
  };
}