#pragma once

#include <string>
#include <chrono>
#include <random>

namespace Lotus
{

  class Randomizer
  {
  public:

    Randomizer(unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count()) :
      engine(seed)
    {}

    float getFloat()
    {
      std::uniform_real_distribution<float> distribution;
      return distribution(engine);
    }

    float getFloatNormalized()
    {
      return getFloatRange(0.0f, 1.0f);
    }

    float getFloatRange(float max)
    {
      return getFloatRange(0.0f, max);
    }

    float getFloatRange(float min, float max)
    {
      std::uniform_real_distribution<float> distribution(min, max);
      return distribution(engine);
    }

    int getInt()
    {
      std::uniform_int_distribution<int> distribution;
      return distribution(engine);
    }

    int getIntRange(int max)
    {
      return getIntRange(0, max);
    }

    int getIntRange(int min, int max)
    {
      std::uniform_int_distribution<int> distribution(min, max);
      return distribution(engine);
    }

    bool getBool()
    {
      return getIntRange(0, 1) ? true : false;
    }

    std::string getString(size_t length)
    {
      static const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
      
      std::uniform_int_distribution<size_t> distribution(0, characters.size() - 1);

      std::string randomString;

      for (size_t i = 0; i < length; ++i)
      {
        randomString += characters[distribution(engine)];
      }

      return randomString;
    }

  private:

    std::mt19937 engine;

  };

}