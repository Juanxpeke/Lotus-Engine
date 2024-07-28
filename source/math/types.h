#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Lotus
{

  /*
    Abstract representation of an unsigned int ID related to an specific class
  */
  template <typename T>
  struct Handler
  {
    uint32_t handle = 0;
  };

}