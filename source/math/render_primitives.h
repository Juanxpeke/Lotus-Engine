#pragma once

#include "types.h"

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
  struct RenderMesh
  {
    uint32_t count;
    uint32_t firstIndex;
    uint32_t baseVertex;
  };

  struct RenderMaterial
  {
    uint32_t ID;
  };

  struct RenderObject
  {
    Handle<RenderMesh> meshHandle = 0;
    Handle<RenderMaterial> materialHandle = 0;
    uint32_t shaderHandle = 0;

    glm::mat4 model;

    uint32_t ID = 0;

    bool unbatched = true;
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
    Handle<RenderMesh> meshHandle;
    uint32_t prevInstanceCount;
    uint32_t instanceCount;
  };

  /*
    Batch for a single object
  */
  struct ObjectBatch
  {
    Handle<RenderObject> objectHandle;
    Handle<int> shaderHandle;
    Handle<RenderMesh> meshHandle;
  };
}