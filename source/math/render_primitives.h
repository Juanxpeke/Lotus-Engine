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
  class Handler
  {
  public:
    Handler() : handle(0) {}
    Handler(uint32_t initHandle) : handle(initHandle) {}

    Handler<T>& operator=(const Handler<T>& other)
    {
      handle = other.handle;
      return *this;
    }

    friend bool operator==(const Handler<T>& l, const Handler<T>& r)
    {
      return l.handle == r.handle;
    }

    friend bool operator<(const Handler<T>& l, const Handler<T>& r)
    {
      return l.handle < r.handle;
    }

    uint32_t get() const { return handle; }

    void set(uint32_t newHandle) { handle = newHandle; }

  private:
    uint32_t handle;
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
    Handler<DrawMesh> meshHandler;
    Handler<int> shaderHandler;

    glm::mat4 model;
  };

  struct PassObject
  {
      // PassMaterial material;
      Handler<DrawMesh> meshHandler;
      Handler<RenderObject> objectHandler;
      int32_t builtbatch;
      uint32_t customKey;
  };

  struct PassMaterial
  {

  };

  /*
    Batch for objects with the same shader
  */
  struct ShaderBatch
  {
    Handler<int> shaderHandler;
		uint32_t first;
		uint32_t count;
  };

  /*
    Batch for objects with the same mesh
  */
  struct DrawBatch
  {
    Handler<int> shaderHandler;
    Handler<DrawMesh> meshHandler;
    uint32_t prevInstanceCount;
    uint32_t instanceCount;
  };

  /*
    Batch for a single object
  */
  struct RenderBatch
  {
    Handler<RenderObject> objectHandler;
    Handler<int> shaderHandler;
    Handler<DrawMesh> meshHandler;
  };
}