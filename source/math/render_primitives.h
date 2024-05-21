#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace Lotus
{  
  /*
    Abstract representation of an unsigned int ID related to an specific class
  */
  template <typename T>
  class Handle
  {
  public:
    Handle() : value(0) {}
    Handle(uint32_t initValue) : value(initValue) {}

    Handle<T>& operator=(const Handle<T>& other)
    {
      value = other.value;
      return *this;
    }

    friend bool operator==(const Handle<T>& l, const Handle<T>& r)
    {
      return l.value == r.value;
    }

    friend bool operator<(const Handle<T>& l, const Handle<T>& r)
    {
      return l.value < r.value;
    }

    uint32_t get() const { return value; }

    void set(uint32_t newValue) { value = newValue; }

  private:
    uint32_t value;
  };

  /*
    Representation of a mesh in the GPU buffers
  */
  struct DrawMesh
  {
      uint32_t count;
      uint32_t firstIndex;
      uint32_t baseVertex;
  };

  struct RenderObject
  {
    Handle<DrawMesh> meshHandle;
    Handle<int> shaderHandle;

    glm::mat4 model;
    uint32_t ID;
  };

  /*
    Batch for objects with the same shader
  */
  struct ShaderBatch
  {
    Handle<int> shaderHandle;
		uint32_t first;
		uint32_t count;
  };

  /*
    Batch for objects with the same mesh
  */
  struct DrawBatch
  {
    Handle<int> shaderHandle;
    Handle<DrawMesh> meshHandle;
    uint32_t prevInstanceCount;
    uint32_t instanceCount;
  };

  /*
    Batch for a single object
  */
  struct RenderBatch
  {
    Handle<RenderObject> objectHandle;
    Handle<int> shaderHandle;
    Handle<DrawMesh> meshHandle;
  };
}